/*!
 * StateGetSelfAuthenticationData.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateGetSelfAuthenticationData.h"

#include "CertificateChecker.h"
#include "HttpStatusCode.h"
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


void StateGetSelfAuthenticationData::reportCommunicationError(const GlobalStatus& pStatus)
{
	qCritical() << pStatus;
	setStatus(pStatus);
	mReply->abort();
	Q_EMIT fireError();
}


void StateGetSelfAuthenticationData::onSslHandshakeDone()
{
	if (!checkSslConnectionAndSaveCertificate(mReply->sslConfiguration()))
	{
		// checkAndSaveCertificate already set the error
		mReply->abort();
	}
}


bool StateGetSelfAuthenticationData::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	qDebug() << "Used session cipher" << pSslConfiguration.sessionCipher();
	qDebug() << "Used session protocol:" << pSslConfiguration.sessionProtocol();
	qDebug() << "Used server certificate:" << pSslConfiguration.peerCertificate();
	qDebug() << "Used ephemeral server key:" << pSslConfiguration.ephemeralServerKey();

	switch (CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), getContext()->getRefreshUrl(), getContext()))
	{
		case CertificateChecker::CertificateStatus::Good:
			break;

		case CertificateChecker::CertificateStatus::Unsupported_Algorithm_Or_Length:
			reportCommunicationError(GlobalStatus(GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length));
			return false;

		case CertificateChecker::CertificateStatus::Hash_Not_In_Description:
			reportCommunicationError(GlobalStatus(GlobalStatus::Code::Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description));
			return false;
	}

	if (!CertificateChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		reportCommunicationError(GlobalStatus(GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length));
		return false;
	}

	return true;
}


void StateGetSelfAuthenticationData::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsConfiguration::containsFatalError(mReply, pErrors))
	{
		reportCommunicationError(GlobalStatus(GlobalStatus::Code::Network_Ssl_Establishment_Error));
	}
}


void StateGetSelfAuthenticationData::onNetworkReply()
{
	int statusCode = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	qDebug() << statusCode;

	if (statusCode == HttpStatusCode::OK)
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
			setStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			Q_EMIT fireError();
		}
	}
	else
	{
		qDebug() << "Could not read data for self authentication. StatusCode:" << statusCode;
		setStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		Q_EMIT fireError();
	}
}
