/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckRefreshAddress.h"

#include "AppSettings.h"
#include "CertificateChecker.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "StateRedirectBrowser.h"
#include "TlsChecker.h"
#include "UrlUtil.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslCipher>
#include <QSslKey>
#include <QSslSocket>
#include <http_parser.h>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)


StateCheckRefreshAddress::StateCheckRefreshAddress(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mReply()
	, mUrl()
	, mSubjectUrl()
	, mCertificateFetched(false)
{
}


bool StateCheckRefreshAddress::isMatchingSameOriginPolicyInDevMode() const
{
	// Checking for same origin policy needs a special treatment in developer mode because
	// a tcTokenURL with the http scheme is acceptable.
	if (!Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
	{
		return false;
	}

	if (mUrl.scheme() == QLatin1String("http") || mSubjectUrl.scheme() == QLatin1String("http"))
	{
		QUrl devModeUrl = mUrl;
		QUrl devModeSubjectUrl = mSubjectUrl;

		// Accept http and ignore redirects, i.e. only compare domains (ignore schemes and ports).
		devModeUrl.setScheme(QStringLiteral("http"));
		devModeSubjectUrl.setScheme(QStringLiteral("http"));
		devModeUrl.setPort(80);
		devModeSubjectUrl.setPort(80);
		const bool matching = UrlUtil::isMatchingSameOriginPolicy(devModeUrl, devModeSubjectUrl);
		if (matching)
		{
			qCCritical(developermode) << "SOP-Check: Ignoring scheme and port in developer mode.";
			qCCritical(developermode).noquote() << "  Origin URL:" << mSubjectUrl.toString();
			qCCritical(developermode).noquote() << "  Refresh URL:" << mUrl.toString();
		}
		return matching;
	}

	return false;
}


void StateCheckRefreshAddress::run()
{
	const auto tcToken = getContext()->getTcToken();

	if (!tcToken)
	{
		qDebug() << "Invalid TCToken";
		Q_EMIT fireContinue();
		return;
	}

	if (tcToken->usePsk())
	{
		// When the tcToken provided a psk, it is necessary to clear
		// all connections to force new connections without psk settings
		// after we finished the authentication communication.
		Env::getSingleton<NetworkManager>()->clearConnections();
	}

	mUrl = tcToken->getRefreshAddress();
	auto refreshAddrError = QStringLiteral("Invalid RefreshAddress: %1").arg(mUrl.toString());
	if (!mUrl.isValid())
	{
		qDebug().noquote() << refreshAddrError;
		Q_EMIT fireContinue();
		return;
	}
	if (mUrl.scheme() != QLatin1String("https"))
	{
		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCDebug(developermode) << refreshAddrError;
		}
		else
		{
			qDebug().noquote() << refreshAddrError;
			Q_EMIT fireContinue();
			return;
		}
	}

	mSubjectUrl = determineSubjectUrl();
	qDebug() << "Subject URL from AT CVC (eService certificate) description:" << mSubjectUrl.toString();
	qDebug() << "Current redirect URL:" << mUrl.toString();

	if (UrlUtil::isMatchingSameOriginPolicy(mSubjectUrl, mUrl) || isMatchingSameOriginPolicyInDevMode())
	{
		qDebug() << "SOP-Check succeeded, abort process.";
		fetchServerCertificate();
	}
	else
	{
		qDebug() << "SOP-Check failed, start process.";
		sendGetRequest();
	}
}


QUrl StateCheckRefreshAddress::determineSubjectUrl() const
{
	QUrl subjectUrl;
	auto eac1 = getContext()->getDidAuthenticateEac1();
	if (eac1 && eac1->getCertificateDescription() != nullptr)
	{
		subjectUrl = QUrl(eac1->getCertificateDescription()->getSubjectUrl(), QUrl::StrictMode);
		if (!subjectUrl.isValid())
		{
			subjectUrl.clear();
		}
	}

	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
	{
		// Perform SOP-Check against TcToken-URL instead of subjectURL
		subjectUrl = getContext()->getTcTokenUrl();
	}

	if (subjectUrl.isEmpty())
	{
		// according to TR-03124, chapter 2.4.5.1
		qWarning() << "No subjectURL/certificate description available, take the TcToken-URL instead";
		subjectUrl = getContext()->getTcTokenUrl();
	}
	return subjectUrl;
}


void StateCheckRefreshAddress::sendGetRequest()
{
	qDebug() << "Send GET request to URL:" << mUrl.toString();
	QNetworkRequest request(mUrl);
	mReply = Env::getSingleton<NetworkManager>()->get(request);

	*this << connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateCheckRefreshAddress::onSslErrors);
	*this << connect(mReply.data(), &QNetworkReply::encrypted, this, &StateCheckRefreshAddress::onSslHandshakeDone);
	*this << connect(mReply.data(), &QNetworkReply::finished, this, &StateCheckRefreshAddress::onNetworkReply);
}


void StateCheckRefreshAddress::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
	{
		reportCommunicationError({GlobalStatus::Code::Network_Ssl_Establishment_Error, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				}, FailureCode::Reason::Check_Refresh_Address_Fatal_Tls_Error_Before_Reply);
	}
}


void StateCheckRefreshAddress::reportCommunicationError(const GlobalStatus& pStatus, const FailureCode& pFailure)
{
	qCritical() << pStatus;
	updateStatus(pStatus);

	clearConnections();
	mReply->abort();

	Q_EMIT fireAbort(pFailure);
}


void StateCheckRefreshAddress::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));
	checkSslConnectionAndSaveCertificate(cfg);
}


bool StateCheckRefreshAddress::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	const QSharedPointer<AuthContext>& context = getContext();
	Q_ASSERT(!context.isNull());

	GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
	};

	if (!TlsChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		const auto& map = TlsChecker::getEphemeralKeyInfoMap(pSslConfiguration.ephemeralServerKey());
		reportCommunicationError({GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length, infoMap},
				{FailureCode::Reason::Check_Refresh_Address_Invalid_Ephemeral_Key_Length, map});
		return false;
	}

	const auto statusCode = CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), mUrl, context);
	if (statusCode == CertificateChecker::CertificateStatus::Good)
	{
		mVerifiedRefreshUrlHosts << mUrl;
		return true;
	}

	infoMap.insert(GlobalStatus::ExternalInformation::CERTIFICATE_ISSUER_NAME, TlsChecker::getCertificateIssuerName(pSslConfiguration.peerCertificate()));
	switch (statusCode)
	{
		case CertificateChecker::CertificateStatus::Good:
			break;

		case CertificateChecker::CertificateStatus::Unsupported_Algorithm_Or_Length:
			reportCommunicationError({CertificateChecker::getGlobalStatus(statusCode, false), infoMap},
					FailureCode::Reason::Check_Refresh_Address_Unsupported_Certificate);
			break;

		case CertificateChecker::CertificateStatus::Hash_Not_In_Description:
			reportCommunicationError({CertificateChecker::getGlobalStatus(statusCode, false), infoMap},
					FailureCode::Reason::Check_Refresh_Address_Hash_Missing_In_Certificate);

	}

	return false;
}


void StateCheckRefreshAddress::onNetworkReply()
{
	const auto statusCode = NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));
	if (mReply->error() != QNetworkReply::NoError)
	{
		const FailureCode::FailureInfoMap infoMap {
			{FailureCode::Info::Network_Error, mReply->errorString()}
		};

		switch (NetworkManager::toNetworkError(mReply))
		{
			case NetworkManager::NetworkError::ServiceUnavailable:
				reportCommunicationError({GlobalStatus::Code::Network_ServiceUnavailable, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Service_Unavailable, infoMap});
				break;

			case NetworkManager::NetworkError::ServerError:
				reportCommunicationError({GlobalStatus::Code::Network_ServerError, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Server_Error, infoMap});
				break;

			case NetworkManager::NetworkError::ClientError:
				reportCommunicationError({GlobalStatus::Code::Network_ClientError, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Client_Error, infoMap});
				break;

			case NetworkManager::NetworkError::TimeOut:
				reportCommunicationError({GlobalStatus::Code::Network_TimeOut, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Service_Timeout, infoMap});
				break;

			case NetworkManager::NetworkError::ProxyError:
				reportCommunicationError({GlobalStatus::Code::Network_Proxy_Error, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Proxy_Error, infoMap});
				break;

			case NetworkManager::NetworkError::SecurityError:
				reportCommunicationError({GlobalStatus::Code::Network_Ssl_Establishment_Error, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Fatal_Tls_Error_After_Reply, infoMap});
				break;

			case NetworkManager::NetworkError::OtherError:
				reportCommunicationError({GlobalStatus::Code::Network_Other_Error, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
						}, {FailureCode::Reason::Check_Refresh_Address_Unknown_Network_Error, infoMap});
				break;
		}
		return;
	}

	if (statusCode != HTTP_STATUS_FOUND && statusCode != HTTP_STATUS_SEE_OTHER && statusCode != HTTP_STATUS_TEMPORARY_REDIRECT)
	{
		qCritical() << "Got unexpected status code:" << statusCode;
		const GlobalStatus::ExternalInfoMap infoMap {
			{GlobalStatus::ExternalInformation::HTTP_STATUS_CODE, QString::number(statusCode)},
			{GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
		};
		reportCommunicationError({GlobalStatus::Code::Workflow_Network_Expected_Redirect, infoMap},
				FailureCode::Reason::Check_Refresh_Address_Invalid_Http_Response);
		return;
	}

	const QUrl& redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (redirectUrl.isEmpty())
	{
		qCritical() << "Got empty redirect URL";
		reportCommunicationError({GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url, {GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
				}, FailureCode::Reason::Check_Refresh_Address_Empty);
		return;
	}

	if (!redirectUrl.isValid())
	{
		qCritical() << "Got malformed redirect URL:" << redirectUrl;
		const GlobalStatus::ExternalInfoMap infoMap {
			{GlobalStatus::ExternalInformation::REDIRECT_URL, redirectUrl.toString()},
			{GlobalStatus::ExternalInformation::LAST_URL, mUrl.toString()}
		};
		reportCommunicationError({GlobalStatus::Code::Workflow_Network_Malformed_Redirect_Url, infoMap},
				FailureCode::Reason::Check_Refresh_Address_Invalid_Url);
		return;
	}

	if (redirectUrl.scheme() != QLatin1String("https"))
	{
		auto httpsError = QStringLiteral("Redirect URL is not https: %1").arg(redirectUrl.toString());

		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << httpsError;
		}
		else
		{
			qCritical() << httpsError;
			const GlobalStatus::ExternalInfoMap infoMap {
				{GlobalStatus::ExternalInformation::URL_SCHEME, redirectUrl.scheme()},
				{GlobalStatus::ExternalInformation::LAST_URL, redirectUrl.toString()}
			};
			reportCommunicationError({GlobalStatus::Code::Workflow_Network_Invalid_Scheme, infoMap},
					FailureCode::Reason::Check_Refresh_Address_No_Https_Scheme);
			return;
		}
	}

	if (UrlUtil::isMatchingSameOriginPolicy(mUrl, mSubjectUrl) || isMatchingSameOriginPolicyInDevMode())
	{
		qDebug() << "SOP-Check succeeded, abort process";
		mUrl = redirectUrl;
		fetchServerCertificate();
	}
	else
	{
		qDebug() << "SOP-Check failed, continue process";
		mUrl = redirectUrl;
		sendGetRequest();
	}
}


void StateCheckRefreshAddress::fetchServerCertificate()
{
	if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode() && mUrl.scheme() == QLatin1String("http"))
	{
		qCWarning(developermode) << "Refresh URL is http only. Certificate check skipped.";
		doneSuccess();
		return;
	}

	/*
	 * According to Tr-03124-1, page 15 fetching the server certificate is optional if the server certificate was already collected:
	 *
	 * "This state MAY be skipped if a connection to the refresh URL was already established as part of the procedure described above"
	 *
	 * But keep attention: ONLY IF THE SERVER CERTIFICATE WAS COLLECTED DURING DETERMINATION OF THE REFRESH URL *NOT* DURING DETERMINATION OF THE TCTOKEN.
	 */
	if (mVerifiedRefreshUrlHosts.contains(mUrl))
	{
		qDebug() << "SSL certificate already collected for" << mUrl;
		doneSuccess();
		return;
	}

	qDebug() << "Fetch TLS certificate for URL" << mUrl;
	QNetworkRequest request(mUrl);

	// Clear all connections to ensure a fresh connection is established and the
	// encrypted signal is emitted, see Qt documentation QNetworkReply::encrypted():
	//
	// "...This means that you are only guaranteed to receive this signal for the first connection to a site in the lifespan of the QNetworkAccessManager."
	Env::getSingleton<NetworkManager>()->clearConnections();
	mReply = Env::getSingleton<NetworkManager>()->get(request);

	*this << connect(mReply.data(), &QNetworkReply::encrypted, this, &StateCheckRefreshAddress::onSslHandshakeDoneFetchingServerCertificate);
	*this << connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateCheckRefreshAddress::onSslErrors);
	*this << connect(mReply.data(), &QNetworkReply::errorOccurred, this, &StateCheckRefreshAddress::onNetworkErrorFetchingServerCertificate);
}


void StateCheckRefreshAddress::onSslHandshakeDoneFetchingServerCertificate()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

	// just establish the TLS connection but do not perform HTTP request
	mCertificateFetched = true;
	mReply->abort();
	if (checkSslConnectionAndSaveCertificate(cfg))
	{
		doneSuccess();
	}
	else
	{
		// checkSslConnectionAndSaveCertificate already set the error
	}
}


void StateCheckRefreshAddress::doneSuccess()
{
	getContext()->setRefreshUrl(mUrl);
	qDebug() << "Determined RefreshUrl:" << mUrl;
	Q_EMIT fireContinue();
}


void StateCheckRefreshAddress::onNetworkErrorFetchingServerCertificate(QNetworkReply::NetworkError pError)
{
	if (mCertificateFetched && pError == QNetworkReply::NetworkError::OperationCanceledError)
	{
		return;
	}
	qCritical() << "An error occurred fetching the server certificate:" << mReply->errorString();
	reportCommunicationError({GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
			}, FailureCode::Reason::Check_Refresh_Address_Fetch_Certificate_Error);
}


void StateCheckRefreshAddress::onEntry(QEvent* pEvent)
{
	//: INFO ALL_PLATFORMS Status message after the communication between card and server is completed, the result is being forwarded to the provider.
	getContext()->setProgress(80, tr("Sending data to provider"));
	AbstractState::onEntry(pEvent);
}


void StateCheckRefreshAddress::onExit(QEvent* pEvent)
{
	if (mReply)
	{
		mReply.reset();
	}

	AbstractState::onExit(pEvent);
}
