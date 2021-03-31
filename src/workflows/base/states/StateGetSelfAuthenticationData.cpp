/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetSelfAuthenticationData.h"

#include "CertificateChecker.h"
#include "LogHandler.h"
#include "SelfAuthenticationData.h"
#include "TlsChecker.h"

#include <http_parser.h>
#include <QLoggingCategory>
#include <QSslKey>

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;

StateGetSelfAuthenticationData::StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
	, mReply()
{
}


void StateGetSelfAuthenticationData::run()
{
	Env::getSingleton<NetworkManager>()->clearConnections();
	QUrl address = getContext()->getRefreshUrl();
	qDebug() << address;

	QNetworkRequest request(address);
	mReply.reset(Env::getSingleton<NetworkManager>()->get(request), &QObject::deleteLater);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGetSelfAuthenticationData::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGetSelfAuthenticationData::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGetSelfAuthenticationData::onNetworkReply);
}


void StateGetSelfAuthenticationData::reportCommunicationError(const GlobalStatus& pStatus)
{
	qCritical() << pStatus;
	updateStatus(pStatus);

	clearConnections();
	mReply->abort();
	Q_EMIT fireAbort();
}


void StateGetSelfAuthenticationData::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	const QSharedPointer<AuthContext>& context = getContext();
	Q_ASSERT(!context.isNull());

	const auto& saveCertificateFunc = [&]
				(const QUrl& pUrl, const QSslCertificate& pCertificate)
			{
				context->addCertificateData(pUrl, pCertificate);
			};

	GlobalStatus::ExternalInfoMap infoMap {
		{GlobalStatus::ExternalInformation::CERTIFICATE_ISSUER_NAME, TlsChecker::getCertificateIssuerName(pSslConfiguration.peerCertificate())}
	};
	if (!mReply.isNull())
	{
		infoMap.insert(GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString());
	}

	switch (CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), getContext()->getRefreshUrl(), context->getDidAuthenticateEac1(), context->getDvCvc(), saveCertificateFunc))
	{
		case CertificateChecker::CertificateStatus::Good:
			break;

		case CertificateChecker::CertificateStatus::Unsupported_Algorithm_Or_Length:
			reportCommunicationError(GlobalStatus(GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length, infoMap));
			return;

		case CertificateChecker::CertificateStatus::Hash_Not_In_Description:
			reportCommunicationError(GlobalStatus(GlobalStatus::Code::Workflow_Network_Ssl_Hash_Not_In_Certificate_Description, infoMap));
			return;
	}

	if (!TlsChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		reportCommunicationError({GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length, infoMap});
	}
}


void StateGetSelfAuthenticationData::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
	{
		GlobalStatus::ExternalInfoMap infoMap;
		if (!mReply.isNull())
		{
			infoMap.insert(GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString());
		}

		reportCommunicationError({GlobalStatus::Code::Network_Ssl_Establishment_Error, infoMap});
	}
}


void StateGetSelfAuthenticationData::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));
	checkSslConnectionAndSaveCertificate(cfg);
}


void StateGetSelfAuthenticationData::onNetworkReply()
{
	qCDebug(network) << "Received SelfAuthData";
	const auto statusCode = NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));

	if (statusCode == HTTP_STATUS_OK)
	{
		const SelfAuthenticationData data(mReply->readAll());
		if (data.isValid())
		{
			getContext()->setSelfAuthenticationData(data);
			Q_EMIT fireContinue();
		}
		else
		{
			qDebug() << "No valid data of self-authentication.";
			updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			Q_EMIT fireAbort();
		}
	}
	else
	{
		qDebug() << "Could not read data for self-authentication.";
		updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
		Q_EMIT fireAbort();
	}
}


void StateGetSelfAuthenticationData::onExit(QEvent* pEvent)
{
	AbstractState::onExit(pEvent);

	if (!mReply.isNull())
	{
		mReply.reset();
	}
}
