/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetSelfAuthenticationData.h"

#include "CertificateChecker.h"
#include "Env.h"
#include "HttpStatusCode.h"
#include "SelfAuthenticationData.h"
#include "TlsChecker.h"

#include <QLoggingCategory>
#include <QSslKey>

Q_DECLARE_LOGGING_CATEGORY(network)

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
	mReply = Env::getSingleton<NetworkManager>()->get(request);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGetSelfAuthenticationData::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGetSelfAuthenticationData::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGetSelfAuthenticationData::onNetworkReply);
}


void StateGetSelfAuthenticationData::reportCommunicationError(const GlobalStatus& pStatus)
{
	qCritical() << pStatus;
	updateStatus(pStatus);
	mReply->abort();
	Q_EMIT fireAbort();
}


void StateGetSelfAuthenticationData::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, qInfo(network));

	if (!checkSslConnectionAndSaveCertificate(cfg))
	{
		// checkAndSaveCertificate already set the error
		mReply->abort();
	}
}


bool StateGetSelfAuthenticationData::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	const QSharedPointer<AuthContext>& context = getContext();
	Q_ASSERT(!context.isNull());

	const auto& saveCertificateFunc = [&]
				(const QUrl& pUrl, const QSslCertificate& pCertificate)
			{
				context->addCertificateData(pUrl, pCertificate);
			};

	switch (CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), getContext()->getRefreshUrl(), context->getDidAuthenticateEac1(), context->getDvCvc(), saveCertificateFunc))
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

	if (!TlsChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		reportCommunicationError(GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length);
		return false;
	}

	return true;
}


void StateGetSelfAuthenticationData::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
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
		const SelfAuthenticationData data(mReply->readAll());
		if (data.isValid())
		{
			getContext()->setSelfAuthenticationData(data);
			Q_EMIT fireContinue();
		}
		else
		{
			qDebug() << "Could not read data for self authentication.";
			updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			Q_EMIT fireAbort();
		}
	}
	else
	{
		qDebug() << "Could not read data for self authentication. StatusCode:" << statusCode;
		updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		Q_EMIT fireAbort();
	}
}
