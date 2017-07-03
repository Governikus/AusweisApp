/*!
 * StateGetTcToken.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateGetTcToken.h"

#include "CertificateChecker.h"
#include "NetworkManager.h"
#include "TlsConfiguration.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QSslKey>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(developermode)


StateGetTcToken::StateGetTcToken(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
	, mReply()
{
}


StateGetTcToken::~StateGetTcToken()
{
	if (!mReply.isNull())
	{
		mReply->deleteLater();
	}
}


void StateGetTcToken::run()
{
	auto url = getContext()->getTcTokenUrl();
	qDebug() << "Got TC Token URL:" << url;

	if (!isValidRedirectUrl(url))
	{
		Q_EMIT fireError();
		return;
	}
	sendRequest(url);
}


bool StateGetTcToken::isValidRedirectUrl(const QUrl& pUrl)
{
	if (pUrl.isEmpty())
	{
		qCritical() << "Error while connecting to the service provider. The server returns an invalid or empty redirect URL.";
		setStatus(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
		return false;
	}
	else if (pUrl.scheme() != QLatin1String("https"))
	{
		// according to TR-03124-1 in case of a non-HTTPS URL a createTrustedChannelEstablishmentError error must be sent
		// in contrast a HTTP error 404 must be sent, if the TCToken could not be determined
		auto httpsError = QStringLiteral("Error while connecting to the service provider. A secure connection could not be established. The used URL is not of type HTTPS.");
		if (AppSettings::getInstance().getGeneralSettings().isDeveloperMode())
		{
			qCCritical(developermode) << httpsError;
		}
		else
		{
			qCritical() << httpsError;
			getContext()->setTcTokenNotFound(false);
			setStatus(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
			return false;
		}
	}
	return true;
}


void StateGetTcToken::sendRequest(const QUrl& pUrl)
{
	if (!mReply.isNull())
	{
		mReply->deleteLater();
	}

	QNetworkRequest request(pUrl);
	mReply = getContext()->getNetworkManager().get(request);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGetTcToken::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGetTcToken::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGetTcToken::onNetworkReply);
}


void StateGetTcToken::onSslErrors(const QList<QSslError>& pErrors)
{
	TlsConfiguration::containsFatalError(mReply, pErrors);
}


void StateGetTcToken::onSslHandshakeDone()
{
	QSslConfiguration sslConfiguration = mReply->sslConfiguration();
	QSslKey ephemeralServerKey = sslConfiguration.ephemeralServerKey();
	qDebug() << "Used session cipher" << sslConfiguration.sessionCipher();
	qDebug() << "Used session protocol:" << sslConfiguration.sessionProtocol();
	qDebug() << "Used ephemeral server key:" << ephemeralServerKey;

	// At this point we can only check the certificate key's length.
	if (!CertificateChecker::hasValidCertificateKeyLength(sslConfiguration.peerCertificate()))
	{
		mReply->abort();
		qCritical() << "Error while connecting to the service provider. The server's SSL certificate uses an unsupported key algorithm or length.";
		setStatus(GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Establishment_Error);
		Q_EMIT fireError();
		return;
	}

	if (!CertificateChecker::hasValidEphemeralKeyLength(ephemeralServerKey))
	{
		mReply->abort();
		qCritical() << "Error while connecting to the service provider. The SSL connection uses an unsupported key algorithm or length.";
		setStatus(GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Establishment_Error);
		Q_EMIT fireError();
		return;
	}

	getContext()->addCertificateData(mReply->request().url(), sslConfiguration.peerCertificate());
}


void StateGetTcToken::onNetworkReply()
{
	int statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	QUrl redirectUrl = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	qDebug() << "Status Code: " << statusCode << " | RedirectUrl: " << redirectUrl;

	if (mReply->error() != QNetworkReply::NoError)
	{
		qCritical() << NetworkManager::toStatus(mReply.data());
		setStatus(NetworkManager::toTrustedChannelStatus(mReply.data()));
		Q_EMIT fireError();
		return;
	}

	if (statusCode == HttpStatusCode::OK)
	{
		parseTcToken();
		return;
	}

	if (!isValidRedirectUrl(redirectUrl))
	{
		Q_EMIT fireError();
		return;
	}

	if (statusCode != HttpStatusCode::SEE_OTHER && statusCode != HttpStatusCode::FOUND && statusCode != HttpStatusCode::TEMPORARY_REDIRECT)
	{
		qCritical() << "Error while connecting to the service provider. The server returns an unexpected status code:" << statusCode;
		setStatus(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
		Q_EMIT fireError();
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
		setStatus(GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received);
		Q_EMIT fireError();
		return;
	}
	QSharedPointer<TcToken> tcToken(new TcToken(data));
	getContext()->setTcToken(tcToken);
	getContext()->setTcTokenNotFound(!tcToken->isSchemaConform());

	if (tcToken->isValid())
	{
		Q_EMIT fireSuccess();
		return;
	}

	qCritical() << "TCToken invalid";
	setStatus(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
	Q_EMIT fireError();
}
