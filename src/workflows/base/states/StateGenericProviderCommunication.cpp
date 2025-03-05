/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateGenericProviderCommunication.h"

#include "CertificateChecker.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "TlsChecker.h"

#include <http_parser.h>

Q_DECLARE_LOGGING_CATEGORY(secure)
Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


StateGenericProviderCommunication::StateGenericProviderCommunication(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mReply()
{
}


void StateGenericProviderCommunication::setProgress() const
{

}


void StateGenericProviderCommunication::run()
{
	setProgress();

	const QUrl address = getRequestUrl();
	qDebug() << address;

	const QByteArray payload = getPayload();

	QNetworkRequest request(address);
	if (payload.isEmpty())
	{
		mReply = Env::getSingleton<NetworkManager>()->get(request);
	}
	else
	{
		request.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("application/json"));
		mReply = Env::getSingleton<NetworkManager>()->post(request, payload);
	}
	*this << connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGenericProviderCommunication::onSslErrors);
	*this << connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGenericProviderCommunication::onSslHandshakeDone);
	*this << connect(mReply.data(), &QNetworkReply::finished, this, &StateGenericProviderCommunication::onNetworkReply);
}


void StateGenericProviderCommunication::reportCommunicationError(const GlobalStatus& pStatus, const FailureCode& pFailure)
{
	qCritical() << pStatus;
	updateStatus(pStatus);

	clearConnections();
	mReply->abort();
	Q_EMIT fireAbort(pFailure);
}


void StateGenericProviderCommunication::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	const QSharedPointer<AuthContext>& context = getContext();
	Q_ASSERT(!context.isNull());

	GlobalStatus::ExternalInfoMap infoMap;
	if (!mReply.isNull())
	{
		infoMap.insert(GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString());
	}

	if (!TlsChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		const GlobalStatus& status {GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length, infoMap};

		auto map = TlsChecker::getEphemeralKeyInfoMap(pSslConfiguration.ephemeralServerKey());
		map.insert(FailureCode::Info::State_Name, getStateName());
		const FailureCode& failure {FailureCode::Reason::Generic_Provider_Communication_Invalid_Ephemeral_Key_Length, map};

		reportCommunicationError(status, failure);
		return;
	}

	const auto statusCode = CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), context->getRefreshUrl(), context);
	if (statusCode != CertificateChecker::CertificateStatus::Good)
	{
		infoMap.insert(GlobalStatus::ExternalInformation::CERTIFICATE_ISSUER_NAME, pSslConfiguration.peerCertificate().issuerDisplayName());
		const auto& status = GlobalStatus(CertificateChecker::getGlobalStatus(statusCode, false), infoMap);
		const FailureCode& failure {FailureCode::Reason::Generic_Provider_Communication_Certificate_Error,
									{
										{FailureCode::Info::Certificate_Status, Enum<CertificateChecker::CertificateStatus>::getName(statusCode)},
										{FailureCode::Info::State_Name, getStateName()}
									}
		};
		reportCommunicationError(status, failure);
	}
}


bool StateGenericProviderCommunication::isLoggingAllowed()
{
	return true;
}


void StateGenericProviderCommunication::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
	{
		GlobalStatus::ExternalInfoMap infoMap;
		if (!mReply.isNull())
		{
			infoMap.insert(GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString());
		}

		const GlobalStatus& status = {GlobalStatus::Code::Network_Ssl_Establishment_Error, infoMap};
		const FailureCode& failure {FailureCode::Reason::Generic_Provider_Communication_Tls_Error,
									{
										{FailureCode::Info::Network_Error, mReply->errorString()},
										{FailureCode::Info::Ssl_Errors, TlsChecker::sslErrorsToString(pErrors)},
										{FailureCode::Info::State_Name, getStateName()}
									}
		};
		reportCommunicationError(status, failure);
	}
}


void StateGenericProviderCommunication::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));
	checkSslConnectionAndSaveCertificate(cfg);
}


void StateGenericProviderCommunication::onNetworkReply()
{
	const auto statusCode = NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));

	if (mReply->error() != QNetworkReply::NoError || statusCode != HTTP_STATUS_OK)
	{
		const auto& status = NetworkManager::toTrustedChannelStatus(mReply);
		qCCritical(network) << GlobalStatus(status);
		updateStatus(status);
		FailureCode::FailureInfoMap infoMap {
			{FailureCode::Info::Network_Error, mReply->errorString()},
			{FailureCode::Info::State_Name, getStateName()}
		};
		if (statusCode > 0)
		{
			infoMap[FailureCode::Info::Http_Status_Code] = QString::number(statusCode);
		}
		FailureCode::Reason reason;
		switch (NetworkManager::toNetworkError(mReply))
		{
			case NetworkManager::NetworkError::ServiceUnavailable:
				reason = FailureCode::Reason::Generic_Provider_Communication_ServiceUnavailable;
				break;

			case NetworkManager::NetworkError::ServerError:
				reason = FailureCode::Reason::Generic_Provider_Communication_Server_Error;
				break;

			case NetworkManager::NetworkError::ClientError:
				reason = FailureCode::Reason::Generic_Provider_Communication_Client_Error;
				break;

			default:
				reason = FailureCode::Reason::Generic_Provider_Communication_Network_Error;
		}
		Q_EMIT fireAbort({reason, infoMap});
		return;
	}

	const QByteArray message = mReply->readAll();
	if (NetworkManager::isLoggingAllowed(mReply) && isLoggingAllowed())
	{
		qCDebug(network).noquote() << "Received raw data:\n" << message;
	}
	else
	{
		if (secure().isDebugEnabled())
		{
			qCDebug(secure).noquote() << "Received raw data:\n" << message;
		}
		else
		{
			qCDebug(network) << "no-log was requested, skip logging of raw data";
		}
	}
	handleNetworkReply(message);
}


void StateGenericProviderCommunication::onExit(QEvent* pEvent)
{
	if (!mReply.isNull())
	{
		mReply.reset();
	}

	AbstractState::onExit(pEvent);
}
