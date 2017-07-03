/*!
 * StateCheckRefreshAddress.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateCheckRefreshAddress.h"

#include "CertificateChecker.h"
#include "NetworkManager.h"
#include "StateRedirectBrowser.h"
#include "TlsConfiguration.h"
#include "UrlUtil.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslCipher>
#include <QSslKey>
#include <QSslSocket>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


StateCheckRefreshAddress::StateCheckRefreshAddress(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
	, mReply(nullptr)
	, mUrl()
	, mSubjectUrl()
	, mNetworkManager()
	, mCertificateFetched(false)
{
}


void StateCheckRefreshAddress::run()
{
	if (!getContext()->getTcToken())
	{
		qDebug() << "Invalid TCToken";
		Q_EMIT fireSuccess();
		return;
	}

	mUrl = getContext()->getTcToken()->getRefreshAddress();
	auto refreshAddrError = QStringLiteral("Invalid RefreshAddress: %1").arg(mUrl.toString());
	if (!mUrl.isValid())
	{
		qDebug() << refreshAddrError;
		Q_EMIT fireSuccess();
		return;
	}
	if (mUrl.scheme() != QLatin1String("https"))
	{
		if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
		{
			qCDebug(developermode) << refreshAddrError;
		}
		else
		{
			qDebug() << refreshAddrError;
			Q_EMIT fireSuccess();
			return;
		}
	}

	qDebug() << "Current URL: " << mUrl.toString();
	mSubjectUrl = determineSubjectUrl();
	qDebug() << "SubjectUrl: " << mSubjectUrl.toString();

	if (UrlUtil::isMatchingSameOriginPolicy(mUrl, mSubjectUrl))
	{
		qDebug() << "SOP-Check succeeded, abort process";
		fetchServerCertificate();
	}
	else
	{
		qDebug() << "SOP-Check failed, start process";
		sendGetRequest();
	}
}


QUrl StateCheckRefreshAddress::determineSubjectUrl()
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

	if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
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
	if (!mReply.isNull())
	{
		mReply->deleteLater();
	}

	qDebug() << "Send GET request to URL: " << mUrl.toString();
	QNetworkRequest request(mUrl);
	mReply = getContext()->getNetworkManager().get(request);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateCheckRefreshAddress::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateCheckRefreshAddress::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateCheckRefreshAddress::onNetworkReply);
}


void StateCheckRefreshAddress::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsConfiguration::containsFatalError(pErrors))
	{
		reportCommunicationError(tr("Failed to establish secure connection"));
	}
	else
	{
		qDebug() << "Ignore SSL errors on QNetworkReply";
		mReply->ignoreSslErrors();
	}
}


void StateCheckRefreshAddress::reportCommunicationError(const QString& pMessage)
{
	qCritical() << pMessage;
	setResult(Result::createCommunicationError(pMessage));
	Q_EMIT fireError();
}


void StateCheckRefreshAddress::onSslHandshakeDone()
{
	if (!checkSslConnectionAndSaveCertificate(mReply->sslConfiguration()))
	{
		// checkAndSaveCertificate already set the error
		mReply->abort();
	}
}


bool StateCheckRefreshAddress::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	qDebug() << "Used session cipher" << pSslConfiguration.sessionCipher();
	qDebug() << "Used session protocol:" << pSslConfiguration.sessionProtocol();
	qDebug() << "Used server certificate:" << pSslConfiguration.peerCertificate();
	qDebug() << "Used ephemeral server key:" << pSslConfiguration.ephemeralServerKey();

	QString errorMsg = CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), mUrl, getContext());
	if (!errorMsg.isNull())
	{
		reportCommunicationError(errorMsg);
		return false;
	}
	if (!CertificateChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		errorMsg = tr("Error while connecting to the service provider. The SSL connection uses an unsupported key algorithm or length.");
		reportCommunicationError(errorMsg);
		return false;
	}

	return true;
}


void StateCheckRefreshAddress::onNetworkReply()
{
	int statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	QUrl redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	qDebug() << "Status Code: " << statusCode << " | redirect URL: " << redirectUrl;

	if (mReply->error() != QNetworkReply::NoError)
	{
		qCritical() << "An error occured: " << mReply->errorString();
		reportCommunicationError(NetworkManager::getLocalizedErrorString(mReply.data()));
		return;
	}
	if (statusCode != 302 && statusCode != 303 && statusCode != 307)
	{
		qCritical() << "Got unexpected status code: " << statusCode;
		reportCommunicationError(tr("Expected redirect, got %1").arg(QString::number(statusCode)));
		return;
	}
	if (redirectUrl.isEmpty())
	{
		qCritical() << "Got empty redirect URL";
		reportCommunicationError(tr("Empty redirect URL"));
		return;
	}
	if (!redirectUrl.isValid())
	{
		qCritical() << "Got malformed redirect URL:" << redirectUrl;
		reportCommunicationError(tr("Malformed redirect URL: %1").arg(QString::fromLatin1(redirectUrl.toEncoded())));
		return;
	}
	if (redirectUrl.scheme() != QLatin1String("https"))
	{
		auto httpsError = QStringLiteral("Redirect URL is not https: %1").arg(redirectUrl.toString());

		if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << httpsError;
		}
		else
		{
			qCritical() << httpsError;
			reportCommunicationError(tr("Invalid scheme: %1").arg(redirectUrl.scheme()));
			return;
		}
	}

	if (UrlUtil::isMatchingSameOriginPolicy(mUrl, mSubjectUrl))
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
	if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode() && mUrl.scheme() == QLatin1String("http"))
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

	//	if (getContext()->containsCertificateFor(mUrl))
	//	{
	//		qDebug() << "SSL certificate already collected for" << mUrl;
	//		doneSuccess();
	//		return;
	//	}

	qDebug() << "Fetch TLS certificate for URL" << mUrl;
	QNetworkRequest request(mUrl);

	// use a separate NetworkManager to ensure a fresh connection is established and the encrypted signal is emitted,
	// see Qt documentation QNetworkReply::encrypted():
	//
	// "...This means that you are only guaranteed to receive this signal for the first connection to a site in the lifespan of the QNetworkAccessManager."
	mReply = mNetworkManager.get(request);

	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateCheckRefreshAddress::onSslHandshakeDoneFetchingServerCertificate);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateCheckRefreshAddress::onSslErrorsFetchingServerCertificate);
	mConnections += connect(mReply.data(), QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), this, &StateCheckRefreshAddress::onNetworkErrorFetchingServerCertificate);
}


void StateCheckRefreshAddress::onSslHandshakeDoneFetchingServerCertificate()
{
	if (checkSslConnectionAndSaveCertificate(mReply->sslConfiguration()))
	{
		doneSuccess();
	}
	else
	{
		// checkSslConnectionAndSaveCertificate already set the error
	}
	// just establish the TLS connection but do not perform HTTP request
	mCertificateFetched = true;
	mReply->abort();
}


void StateCheckRefreshAddress::doneSuccess()
{
	getContext()->setRefreshUrl(mUrl);
	qDebug() << "Determined RefreshUrl: " << mUrl;

	Q_EMIT fireSuccess();
}


void StateCheckRefreshAddress::onSslErrorsFetchingServerCertificate(const QList<QSslError>& pErrors)
{
	if (TlsConfiguration::containsFatalError(pErrors))
	{
		reportCommunicationError(tr("Failed to establish secure connection"));
	}
	else
	{
		qDebug() << "Ignore SSL errors on QSslSocket";
		mReply->ignoreSslErrors();
	}
}


void StateCheckRefreshAddress::onNetworkErrorFetchingServerCertificate(QNetworkReply::NetworkError pError)
{
	if (mCertificateFetched && pError == QNetworkReply::NetworkError::OperationCanceledError)
	{
		return;
	}
	qCritical() << "An error occured fetching the server certificate: " << mReply->errorString();
	reportCommunicationError(tr("Failed to establish secure connection"));
}
