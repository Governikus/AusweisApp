/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetTcToken.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "TlsChecker.h"

#include <QLoggingCategory>
#include <QNetworkRequest>
#include <QSslKey>
#include <http_parser.h>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)


StateGetTcToken::StateGetTcToken(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mReply()
{
}


void StateGetTcToken::run()
{
	const auto& url = getContext()->getTcTokenUrl();
	getContext()->setProgress(0, tr("Fetch TCToken"));

	if (!isValidRedirectUrl(url))
	{
		qCCritical(network) << "TCToken URL is invalid:" << url;
		Q_EMIT fireAbort(FailureCode::Reason::Get_TcToken_Invalid_Url);
		return;
	}

	// At this time we start the first network communication in a
	// workflow. Therefore it is necessary to clear all connections
	// to force new connections with the expected security settings.
	Env::getSingleton<NetworkManager>()->clearConnections();
	sendRequest(url);
}


bool StateGetTcToken::isValidRedirectUrl(const QUrl& pUrl)
{
	if (pUrl.isEmpty())
	{
		qCCritical(network) << "Error while connecting to the provider. The server returns an invalid or empty redirect URL.";
		updateStatus(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
		return false;
	}
	else if (pUrl.scheme() != QLatin1String("https"))
	{
		const auto httpsError1 = QStringLiteral("Error while connecting to the provider. A secure connection could not be established.");
		const auto httpsError2 = QStringLiteral("The used URL is not of type HTTPS: %1").arg(pUrl.toString());
		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << httpsError1;
			qCCritical(developermode) << httpsError2;
		}
		else
		{
			qCCritical(network) << httpsError1;
			qCCritical(network) << httpsError2;
			// according to TR-03124-1 in case of a non-HTTPS URL a createTrustedChannelEstablishmentError error must be sent
			// in contrast a HTTP error 404 must be sent, if the TCToken could not be determined
			getContext()->setTcTokenNotFound(false);
			const GlobalStatus::ExternalInfoMap& infoMap {
				{GlobalStatus::ExternalInformation::LAST_URL, pUrl.toString()},
				{GlobalStatus::ExternalInformation::URL_SCHEME, pUrl.scheme()}
			};
			updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error, infoMap});
			return false;
		}
	}
	return true;
}


void StateGetTcToken::sendRequest(const QUrl& pUrl)
{
	qCDebug(network) << "Fetch TCToken URL:" << pUrl;
	QNetworkRequest request(pUrl);
	mReply = Env::getSingleton<NetworkManager>()->get(request);
	*this << connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGetTcToken::onSslErrors);
	*this << connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGetTcToken::onSslHandshakeDone);
	*this << connect(mReply.data(), &QNetworkReply::finished, this, &StateGetTcToken::onNetworkReply);
}


void StateGetTcToken::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
	{
		mReply->abort();
	}
}


void StateGetTcToken::onExit(QEvent* pEvent)
{
	if (!mReply.isNull())
	{
		mReply.reset();
	}

	AbstractState::onExit(pEvent);
}


void StateGetTcToken::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

	const GlobalStatus status {GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
	};
	// At this point we can only check the certificate key's length.
	if (!TlsChecker::hasValidCertificateKeyLength(cfg.peerCertificate()))
	{
		clearConnections();
		mReply->abort();
		qCCritical(network) << "Error while connecting to the provider. The server's SSL certificate uses an unsupported key algorithm or length.";
		updateStatus(status);
		Q_EMIT fireAbort(FailureCode::Reason::Get_TcToken_Invalid_Certificate_Key_Length);
		return;
	}

	if (!TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey()))
	{
		clearConnections();
		mReply->abort();
		qCCritical(network) << "Error while connecting to the provider. The SSL connection uses an unsupported key algorithm or length.";
		updateStatus(status);

		const auto& map = TlsChecker::getEphemeralKeyInfoMap(cfg.ephemeralServerKey());
		Q_EMIT fireAbort({FailureCode::Reason::Get_TcToken_Invalid_Ephemeral_Key_Length, map});
		return;
	}

	auto context = getContext();
	context->addCertificateData(mReply->request().url(), cfg.peerCertificate());
	context->setSslSession(cfg.sessionTicket());
}


void StateGetTcToken::onNetworkReply()
{
	qCDebug(network) << "TCToken request finished";
	const auto statusCode = NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));

	if (mReply->error() != QNetworkReply::NoError)
	{
		qCCritical(network) << NetworkManager::toStatus(mReply);
		updateStatus(NetworkManager::toTrustedChannelStatus(mReply));
		Q_EMIT fireAbort({FailureCode::Reason::Get_TcToken_Network_Error,
						  {FailureCode::Info::Network_Error, mReply->errorString()}
				});
		return;
	}

	if (statusCode == HTTP_STATUS_OK)
	{
		qCDebug(network) << "Received TCToken from eID-Service";
		parseTcToken();
		return;
	}

	if (statusCode == HTTP_STATUS_SEE_OTHER || statusCode == HTTP_STATUS_FOUND || statusCode == HTTP_STATUS_TEMPORARY_REDIRECT)
	{
		const QUrl& redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
		if (!isValidRedirectUrl(redirectUrl))
		{
			Q_EMIT fireAbort(FailureCode::Reason::Get_TcToken_Invalid_Redirect_Url);
			return;
		}
		sendRequest(redirectUrl);
		return;
	}

	qCritical() << "Error while connecting to the provider. The server returns an unexpected status code:" << statusCode;

	GlobalStatus::Code errorStatus = GlobalStatus::Code::No_Error;
	FailureCode::Reason reason;
	if (statusCode >= 500)
	{
		if (statusCode == 503)
		{
			errorStatus = GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable;
			reason = FailureCode::Reason::Get_TcToken_ServiceUnavailable;
		}
		else
		{
			errorStatus = GlobalStatus::Code::Workflow_TrustedChannel_Server_Error;
			reason = FailureCode::Reason::Get_TcToken_Server_Error;
		}
	}
	else if (statusCode >= 400)
	{
		errorStatus = GlobalStatus::Code::Workflow_TrustedChannel_Client_Error;
		reason = FailureCode::Reason::Get_TcToken_Client_Error;
	}
	else
	{
		errorStatus = GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error;
		reason = FailureCode::Reason::Get_TcToken_Invalid_Server_Reply;
	}

	const GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::HTTP_STATUS_CODE, QString::number(statusCode)},
		{GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
	};
	updateStatus({errorStatus, infoMap});
	Q_EMIT fireAbort({reason,
					  {FailureCode::Info::Http_Status_Code, QString::number(statusCode)}
			});
}


void StateGetTcToken::parseTcToken()
{
	Q_ASSERT(getContext()->getTcToken().isNull());

	if (mReply->bytesAvailable() == 0)
	{
		qCDebug(network) << "Received no data.";
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		Q_EMIT fireAbort(FailureCode::Reason::Get_TcToken_Empty_Data);
		return;
	}
	const auto& tcToken = QSharedPointer<TcToken>::create(mReply);
	getContext()->setTcToken(tcToken);
	getContext()->setTcTokenNotFound(!tcToken->isSchemaConform());

	if (tcToken->isValid())
	{
		if (tcToken->usePsk())
		{
			// When the tcToken provides a psk, it is necessary to clear
			// all connections to force new connections with psk settings.
			Env::getSingleton<NetworkManager>()->clearConnections();
		}
		Q_EMIT fireContinue();
		return;
	}

	qCCritical(network) << "TCToken invalid";
	updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
			});
	Q_EMIT fireAbort(FailureCode::Reason::Get_TcToken_Invalid_Data);
}
