/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetTcToken.h"

#include "AppSettings.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "TlsChecker.h"

#include <http_parser.h>
#include <QDebug>
#include <QNetworkRequest>
#include <QSslKey>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)


StateGetTcToken::StateGetTcToken(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
	, mReply()
{
}


void StateGetTcToken::run()
{
	auto url = getContext()->getTcTokenUrl();
	qDebug() << "Got TC Token URL:" << url;

	if (!isValidRedirectUrl(url))
	{
		Q_EMIT fireAbort();
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
		qCritical() << "Error while connecting to the provider. The server returns an invalid or empty redirect URL.";
		updateStatus(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
		return false;
	}
	else if (pUrl.scheme() != QLatin1String("https"))
	{
		// according to TR-03124-1 in case of a non-HTTPS URL a createTrustedChannelEstablishmentError error must be sent
		// in contrast a HTTP error 404 must be sent, if the TCToken could not be determined
		const auto httpsError1 = QStringLiteral("Error while connecting to the provider. A secure connection could not be established.");
		const auto httpsError2 = QStringLiteral("  The used URL is not of type HTTPS: %1").arg(pUrl.toString());
		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << httpsError1;
			qCCritical(developermode) << httpsError2;
		}
		else
		{
			qCritical() << httpsError1;
			qCritical() << httpsError2;
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
	QNetworkRequest request(pUrl);
	mReply.reset(Env::getSingleton<NetworkManager>()->get(request), &QObject::deleteLater);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGetTcToken::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGetTcToken::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGetTcToken::onNetworkReply);
}


void StateGetTcToken::onSslErrors(const QList<QSslError>& pErrors)
{
	TlsChecker::containsFatalError(mReply, pErrors);
}


void StateGetTcToken::onExit(QEvent* pEvent)
{
	AbstractState::onExit(pEvent);

	if (!mReply.isNull())
	{
		mReply.reset();
	}
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
		qCritical() << "Error while connecting to the provider. The server's SSL certificate uses an unsupported key algorithm or length.";
		updateStatus(status);
		Q_EMIT fireAbort();
		return;
	}

	if (!TlsChecker::hasValidEphemeralKeyLength(cfg.ephemeralServerKey()))
	{
		clearConnections();
		mReply->abort();
		qCritical() << "Error while connecting to the provider. The SSL connection uses an unsupported key algorithm or length.";
		updateStatus(status);
		Q_EMIT fireAbort();
		return;
	}

	auto context = getContext();
	context->addCertificateData(mReply->request().url(), cfg.peerCertificate());
	context->setSslSession(cfg.sessionTicket());
}


void StateGetTcToken::onNetworkReply()
{
	qCDebug(network) << "Received TCToken from eID-Service";
	const auto statusCode = NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));

	if (mReply->error() != QNetworkReply::NoError)
	{
		qCritical() << NetworkManager::toStatus(mReply);
		updateStatus(NetworkManager::toTrustedChannelStatus(mReply));
		Q_EMIT fireAbort();
		return;
	}

	if (statusCode == HTTP_STATUS_OK)
	{
		parseTcToken();
		return;
	}

	const QUrl& redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!isValidRedirectUrl(redirectUrl))
	{
		Q_EMIT fireAbort();
		return;
	}

	if (statusCode != HTTP_STATUS_SEE_OTHER && statusCode != HTTP_STATUS_FOUND && statusCode != HTTP_STATUS_TEMPORARY_REDIRECT)
	{
		qCritical() << "Error while connecting to the provider. The server returns an unexpected status code:" << statusCode;
		const GlobalStatus::ExternalInfoMap infoMap {
			{GlobalStatus::ExternalInformation::HTTP_STATUS_CODE, QString::number(statusCode)},
			{GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
		};
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error, infoMap});
		Q_EMIT fireAbort();
		return;
	}

	sendRequest(redirectUrl);
}


void StateGetTcToken::parseTcToken()
{
	Q_ASSERT(getContext()->getTcToken().isNull());

	QByteArray data = mReply->readAll();
	if (data.isEmpty())
	{
		qDebug() << "Received no data.";
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		Q_EMIT fireAbort();
		return;
	}
	const QSharedPointer<const TcToken>& tcToken = QSharedPointer<TcToken>::create(data);
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

	qCritical() << "TCToken invalid";
	updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
			});
	Q_EMIT fireAbort();
}
