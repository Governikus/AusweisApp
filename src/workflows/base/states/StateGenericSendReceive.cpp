/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateGenericSendReceive.h"

#include "AppSettings.h"
#include "CertificateChecker.h"
#include "Env.h"
#include "LogHandler.h"
#include "TlsChecker.h"
#include "paos/PaosHandler.h"

Q_DECLARE_LOGGING_CATEGORY(secure)
Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


StateGenericSendReceive::StateGenericSendReceive(
	const QSharedPointer<WorkflowContext>& pContext,
	PaosType pExpectedResponseType,
	const QVector<PaosType>& pOtherResponseTypes,
	bool pPersonalization)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
	, mExpectedResponseType(pExpectedResponseType)
	, mOtherResponseTypes(pOtherResponseTypes)
	, mPersonalization(pPersonalization)
	, mReply()
{
}


void StateGenericSendReceive::emitStateMachineSignal(PaosType pResponseType)
{
	Q_UNUSED(pResponseType)
	const FailureCode::FailureInfoMap infoMap {
		{FailureCode::Info::State_Name, getStateName()},
		{FailureCode::Info::Paos_Type, Enum<PaosType>::getName(pResponseType)}
	};
	Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Paos_Unhandled, infoMap});
}


void StateGenericSendReceive::setReceivedMessage(const QSharedPointer<PaosMessage>& pMessage)
{
	getContext()->setReceivedMessageId(pMessage->getMessageId());

	switch (pMessage->mType)
	{
		case PaosType::STARTPAOS_RESPONSE:
			getContext()->setStartPaosResponse(pMessage.staticCast<StartPaosResponse>());
			// no need to create a PAOS response message
			break;

		case PaosType::INITIALIZE_FRAMEWORK:
			getContext()->setInitializeFramework(pMessage.staticCast<InitializeFramework>());
			getContext()->setInitializeFrameworkResponse(QSharedPointer<InitializeFrameworkResponse>::create());
			break;

		case PaosType::DID_AUTHENTICATE_EAC1:
			getContext()->setDidAuthenticateEac1(pMessage.staticCast<DIDAuthenticateEAC1>());
			getContext()->setDidAuthenticateResponseEac1(QSharedPointer<DIDAuthenticateResponseEAC1>::create());
			break;

		case PaosType::DID_AUTHENTICATE_EAC2:
			getContext()->setDidAuthenticateEac2(pMessage.staticCast<DIDAuthenticateEAC2>());
			getContext()->setDidAuthenticateResponseEac2(QSharedPointer<DIDAuthenticateResponseEAC2>::create());
			getContext()->setDidAuthenticateResponseEacAdditionalInputType(QSharedPointer<DIDAuthenticateResponseEAC2>::create());
			break;

		case PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE:
			getContext()->setDidAuthenticateEacAdditional(pMessage.staticCast<DIDAuthenticateEACAdditional>());
			// no need to create a PAOS response message. The response is of type DIDAuthenticateResponseEAC2 and is already created, see above
			break;

		case PaosType::TRANSMIT:
			getContext()->setTransmit(pMessage.staticCast<Transmit>());
			getContext()->setTransmitResponse(QSharedPointer<TransmitResponse>::create());
			break;

		default:
			qCWarning(network) << "Unknown received message type:" << static_cast<int>(pMessage->mType);
			break;
	}
}


void StateGenericSendReceive::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error);
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});

		clearConnections();
		mReply->abort();
		const FailureCode::FailureInfoMap infoMap {
			{FailureCode::Info::State_Name, getStateName()},
			{FailureCode::Info::Ssl_Errors, TlsChecker::sslErrorsToString(pErrors)}
		};
		Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Ssl_Error, infoMap});
	}
}


void StateGenericSendReceive::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

	auto failure = checkSslConnectionAndSaveCertificate(cfg);

	auto context = getContext();
	if (!failure.has_value() && !context->getTcToken()->usePsk())
	{
		const auto& session = context->getSslSession();
		if (session.isEmpty() || session != cfg.sessionTicket())
		{
			const auto& sessionFailedError = "Session resumption failed";

			if (Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode())
			{
				qCCritical(developermode) << sessionFailedError;
			}
			else
			{
				qCCritical(network) << sessionFailedError;
				updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
						});
				failure = {FailureCode::Reason::Generic_Send_Receive_Session_Resumption_Failed,
						   {FailureCode::Info::State_Name, getStateName()}
				};
			}
		}
	}

	if (failure.has_value())
	{
		clearConnections();
		mReply->abort();
		Q_EMIT fireAbort(failure.value());
	}
}


void StateGenericSendReceive::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(network) << "pre-shared key authentication requested:" << pAuthenticator->identityHint();

	auto token = getContext()->getTcToken();
	Q_ASSERT(!token.isNull());
	if (token->usePsk())
	{
		pAuthenticator->setIdentity(token->getSessionIdentifier());
		pAuthenticator->setPreSharedKey(QByteArray::fromHex(token->getPsk()));
	}
}


void StateGenericSendReceive::onExit(QEvent* pEvent)
{
	if (!mReply.isNull())
	{
		mReply.reset();
	}

	AbstractState::onExit(pEvent);
}


std::optional<FailureCode> StateGenericSendReceive::checkSslConnectionAndSaveCertificate(const QSslConfiguration& pSslConfiguration)
{
	const QSharedPointer<AuthContext> context = getContext();
	Q_ASSERT(!context.isNull());

	GlobalStatus::ExternalInfoMap infoMap;
	if (!mReply.isNull())
	{
		infoMap.insert(GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString());
	}

	if (!context->getTcToken()->usePsk() &&
			!TlsChecker::hasValidEphemeralKeyLength(pSslConfiguration.ephemeralServerKey()))
	{
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Connection_Unsupported_Algorithm_Or_Length, infoMap});
		return FailureCode(FailureCode::Reason::Generic_Send_Receive_Invalid_Ephemeral_Key_Length,
				{FailureCode::Info::State_Name, getStateName()});
	}

	const auto statusCode = CertificateChecker::checkAndSaveCertificate(pSslConfiguration.peerCertificate(), context->getTcToken()->getServerAddress(), context);
	if (statusCode != CertificateChecker::CertificateStatus::Good)
	{
		infoMap.insert(GlobalStatus::ExternalInformation::CERTIFICATE_ISSUER_NAME, TlsChecker::getCertificateIssuerName(pSslConfiguration.peerCertificate()));
		updateStatus({CertificateChecker::getGlobalStatus(statusCode, true), infoMap});
		const FailureCode::FailureInfoMap failureInfoMap {
			{FailureCode::Info::State_Name, getStateName()},
			{FailureCode::Info::Certificate_Status, Enum<CertificateChecker::CertificateStatus>::getName(statusCode)}
		};
		return FailureCode(FailureCode::Reason::Generic_Send_Receive_Certificate_Error, failureInfoMap);
	}

	return {};
}


void StateGenericSendReceive::run()
{
	getAsCreator()->setRelatedMessageId(getContext()->getReceivedMessageId());

	if (QSharedPointer<ResponseType> paosResponse = getAsResponse())
	{
		if (!getContext()->isErrorReportedToServer() && getContext()->getStatus().isError() && !getContext()->getStatus().isOriginServer())
		{
			auto ctx = getContext();
			if (!ctx->getStartPaosResult().isOk())
			{
				paosResponse->setResult(ctx->getStartPaosResult());
			}
			else
			{
				paosResponse->setResult(ECardApiResult(ctx->getStatus()));
			}
			ctx->setErrorReportedToServer(true);
		}
		else
		{
			paosResponse->setResult(ECardApiResult::createOk());
		}
	}

	Q_ASSERT(mReply.isNull());
	auto token = getContext()->getTcToken();
	Q_ASSERT(!token.isNull());

	QUrl serverAddress = token->getServerAddress();
	if (mPersonalization)
	{
		serverAddress.setPath(QStringLiteral("/personalization"));
		qCDebug(network) << "Using generated serverAddress for personalization:" << serverAddress;
	}

	QNetworkRequest request(serverAddress);
	request.setRawHeader("requestid", token->getSessionIdentifier());
	const QByteArray& data = getAsCreator()->marshall();
	Q_ASSERT(!data.isEmpty());

	qCDebug(network).noquote() << "Try to send raw data:\n" << data;
	const QByteArray& paosNamespace = PaosCreator::getNamespace(PaosCreator::Namespace::PAOS).toUtf8();
	const auto& session = token->usePsk() ? QByteArray() : getContext()->getSslSession();
	mReply = Env::getSingleton<NetworkManager>()->paos(request, paosNamespace, data, token->usePsk(), session);
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGenericSendReceive::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGenericSendReceive::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGenericSendReceive::onReplyFinished);
	mConnections += connect(mReply.data(), &QNetworkReply::preSharedKeyAuthenticationRequired, this, &StateGenericSendReceive::onPreSharedKeyAuthenticationRequired);
}


void StateGenericSendReceive::onReplyFinished()
{
	qCDebug(network) << "Received message from eID-Server";
	const auto statusCode = NetworkManager::getLoggedStatusCode(mReply, spawnMessageLogger(network));

	if (mReply->error() != QNetworkReply::NoError)
	{
		const auto& channelStatus = NetworkManager::toTrustedChannelStatus(mReply);
		qCCritical(network) << GlobalStatus(channelStatus);
		updateStatus(channelStatus);
		const FailureCode::FailureInfoMap infoMap {
			{FailureCode::Info::Network_Error, mReply->errorString()},
			{FailureCode::Info::State_Name, getStateName()}
		};
		Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Network_Error, infoMap});
		return;
	}

	if (statusCode >= 500)
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server);
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		const FailureCode::FailureInfoMap infoMap {
			{FailureCode::Info::State_Name, getStateName()},
			{FailureCode::Info::Http_Status_Code, QString::number(statusCode)}
		};
		Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Server_Error, infoMap});
		return;
	}

	if (statusCode >= 400)
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer);
		updateStatus({GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		const FailureCode::FailureInfoMap infoMap {
			{FailureCode::Info::State_Name, getStateName()},
			{FailureCode::Info::Http_Status_Code, QString::number(statusCode)}
		};
		Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Client_Error, infoMap});
		return;
	}

	QByteArray message = mReply->readAll();
	if (NetworkManager::isLoggingAllowed(mReply))
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

	PaosHandler paosHandler(message);
	const auto receivedType = paosHandler.getDetectedPaosType();
	qCDebug(network) << "Received PAOS message of type:" << receivedType;

	if (mExpectedResponseType == receivedType)
	{
		setReceivedMessage(paosHandler.getPaosMessage());
		Q_EMIT fireContinue();
		return;
	}

	if (receivedType == PaosType::STARTPAOS_RESPONSE)
	{
		setReceivedMessage(paosHandler.getPaosMessage());
		Q_EMIT fireReceivedStartPaosResponse();
		return;
	}

	if (mOtherResponseTypes.contains(receivedType))
	{
		setReceivedMessage(paosHandler.getPaosMessage());
		emitStateMachineSignal(receivedType);
		return;
	}

	qCCritical(network) << "Received PAOS message not of expected type:" << mExpectedResponseType << mOtherResponseTypes;
	if (receivedType == PaosType::UNKNOWN)
	{
		qCCritical(network) << "The program received an unknown message from the server.";
		updateStatus({GlobalStatus::Code::Workflow_Unknown_Paos_From_EidServer, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Paos_Unknown,
						  {FailureCode::Info::State_Name, getStateName()}
				});
		return;
	}

	qCCritical(network) << "The program received an unexpected message from the server.";
	updateStatus({GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
			});
	const FailureCode::FailureInfoMap infoMap {
		{FailureCode::Info::State_Name, getStateName()},
		{FailureCode::Info::Paos_Type, Enum<PaosType>::getName(receivedType)}
	};
	Q_EMIT fireAbort({FailureCode::Reason::Generic_Send_Receive_Paos_Unexpected, infoMap});
}
