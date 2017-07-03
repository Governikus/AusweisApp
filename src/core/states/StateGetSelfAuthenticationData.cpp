/*!
 * StateGetSelfAuthenticationData.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateGetSelfAuthenticationData.h"

#include "CertificateChecker.h"
#include "SelfAuthenticationData.h"
#include "TlsConfiguration.h"

#include <QDebug>
#include <QSslKey>

using namespace governikus;

StateGetSelfAuthenticationData::StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
	, mReply(nullptr)
{
}


StateGetSelfAuthenticationData::~StateGetSelfAuthenticationData()
{
	if (!mReply.isNull())
	{
		mReply->deleteLater();
	}
}


void StateGetSelfAuthenticationData::run()
{
	QUrl address = getContext()->getRefreshUrl();
	qDebug() << address;

	QNetworkRequest request(address);
	mReply = getContext()->getNetworkManager().get(request);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGetSelfAuthenticationData::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGetSelfAuthenticationData::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGetSelfAuthenticationData::onNetworkReply);
}


void StateGetSelfAuthenticationData::reportCommunicationError(const QString& pMessage)
{
	qCritical() << pMessage;
	setResult(Result::createCommunicationError(pMessage));
	mReply->abort();
	Q_EMIT fireError();
}


void StateGetSelfAuthenticationData::onSslHandshakeDone()
{
	const QSslConfiguration sslConfiguration = mReply->sslConfiguration();
	qDebug() << "Used session cipher" << sslConfiguration.sessionCipher();
	qDebug() << "Used session protocol:" << sslConfiguration.sessionProtocol();
	qDebug() << "Used server certificate:" << sslConfiguration.peerCertificate();
	qDebug() << "Used ephemeral server key:" << sslConfiguration.ephemeralServerKey();

	QString errorMsg = CertificateChecker::checkAndSaveCertificate(sslConfiguration.peerCertificate(), getContext()->getRefreshUrl(), getContext());
	if (!errorMsg.isNull())
	{
		reportCommunicationError(errorMsg);
	}

	if (!CertificateChecker::hasValidEphemeralKeyLength(sslConfiguration.ephemeralServerKey()))
	{
		errorMsg = tr("Error while connecting to the service provider. The SSL connection uses an unsupported key algorithm or length.");
		reportCommunicationError(errorMsg);
	}
}


void StateGetSelfAuthenticationData::onSslErrors(const QList<QSslError>& pErrors)
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


void StateGetSelfAuthenticationData::onNetworkReply()
{
	int statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	qDebug() << statusCode;

	if (statusCode == 200)
	{
		QSharedPointer<SelfAuthenticationData> data(new SelfAuthenticationData(mReply->readAll()));
		if (data->isValid())
		{
			getContext()->setSelfAuthenticationData(data);
			Q_EMIT fireSuccess();
		}
		else
		{
			qDebug() << "Could not read data for self authentication.";
			setResult(Result::createCommunicationError(tr("The server provided no or incomplete information. Your personal data could not be read out.")));
			Q_EMIT fireError();
		}
	}
	else
	{
		qDebug() << "Could not read data for self authentication. StatusCode:" << statusCode;
		setResult(Result::createCommunicationError(tr("The server provided no or incomplete information. Your personal data could not be read out.")));
		Q_EMIT fireError();
	}
}
