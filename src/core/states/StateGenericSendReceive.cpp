/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CertificateChecker.h"
#include "StateGenericSendReceive.h"
#include "TlsConfiguration.h"
#include "paos/PaosHandler.h"

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


StateGenericSendReceive::StateGenericSendReceive(QSharedPointer<WorkflowContext> pContext, const QVector<PaosType>& pTypesToReceive)
	: AbstractGenericState(pContext)
	, mTypesToReceive(pTypesToReceive)
	, mReply()
{
}


StateGenericSendReceive::StateGenericSendReceive(QSharedPointer<WorkflowContext> pContext, PaosType pTypesToReceive)
	: StateGenericSendReceive(pContext, QVector<PaosType>() << pTypesToReceive)
{
}


void StateGenericSendReceive::setReceivedMessage(QSharedPointer<PaosMessage> pMessage)
{
	switch (pMessage->mType)
	{
		case PaosType::STARTPAOS_RESPONSE:
			getContext()->setStartPaosResponse(pMessage.staticCast<StartPaosResponse>());
			// no need to create a PAOS response message
			break;

		case PaosType::INITIALIZE_FRAMEWORK:
			getContext()->setInitializeFramework(pMessage.staticCast<InitializeFramework>());
			getContext()->setInitializeFrameworkResponse(QSharedPointer<InitializeFrameworkResponse>(new InitializeFrameworkResponse()));
			break;

		case PaosType::DID_LIST:
			getContext()->setDidList(pMessage.staticCast<DIDList>());
			getContext()->setDidListResponse(QSharedPointer<DIDListResponse>(new DIDListResponse()));
			break;

		case PaosType::DID_AUTHENTICATE_EAC1:
			getContext()->setDidAuthenticateEac1(pMessage.staticCast<DIDAuthenticateEAC1>());
			getContext()->setDidAuthenticateResponseEac1(QSharedPointer<DIDAuthenticateResponseEAC1>(new DIDAuthenticateResponseEAC1()));
			break;

		case PaosType::DID_AUTHENTICATE_EAC2:
			getContext()->setDidAuthenticateEac2(pMessage.staticCast<DIDAuthenticateEAC2>());
			getContext()->setDidAuthenticateResponseEac2(QSharedPointer<DIDAuthenticateResponseEAC2>(new DIDAuthenticateResponseEAC2()));
			getContext()->setDidAuthenticateResponseEacAdditionalInputType(QSharedPointer<DIDAuthenticateResponseEAC2>(new DIDAuthenticateResponseEAC2()));
			break;

		case PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE:
			getContext()->setDidAuthenticateEacAdditional(pMessage.staticCast<DIDAuthenticateEACAdditional>());
			// no need to create a PAOS response message. The response is of type DIDAuthenticateResponseEAC2 and is already created, see above
			break;

		case PaosType::TRANSMIT:
			getContext()->addTransmit(pMessage.staticCast<Transmit>());
			getContext()->addTransmitResponse(QSharedPointer<TransmitResponse>(new TransmitResponse()));
			break;

		case PaosType::DISCONNECT:
			getContext()->setDisconnectResponse(QSharedPointer<DisconnectResponse>(new DisconnectResponse()));
			break;

		default:
			qCWarning(network) << "Unknown received message type:" << static_cast<int>(pMessage->mType);
			break;
	}
}


void StateGenericSendReceive::setMessageId(QSharedPointer<PaosMessage> pPaosMessage)
{
	QSharedPointer<MessageIdHandler> messageIdHandler = getContext()->getMessageIdHandler();
	pPaosMessage->setRelatesTo(messageIdHandler->getRemoteMsgId());
	pPaosMessage->setMessageId(messageIdHandler->createNewMsgId());
}


void StateGenericSendReceive::setRemoteMessageId(QSharedPointer<PaosMessage> pPaosMessage)
{
	QSharedPointer<MessageIdHandler> messageIdHandler = getContext()->getMessageIdHandler();
	messageIdHandler->setRemoteMsgId(pPaosMessage->getMessageId());
}


void StateGenericSendReceive::reportChannelEstablishmentError(const GlobalStatus::Code pStatusCode)
{
	qCCritical(network) << GlobalStatus(pStatusCode);
	setStatus(pStatusCode);
	Q_EMIT fireError();
}


void StateGenericSendReceive::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsConfiguration::containsFatalError(mReply, pErrors))
	{
		reportChannelEstablishmentError(GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error);
	}
}


void StateGenericSendReceive::onSslHandshakeDone()
{
	QSslConfiguration sslConfiguration = mReply->sslConfiguration();
	qCDebug(network) << "Used session cipher" << sslConfiguration.sessionCipher();
	qCDebug(network) << "Used session protocol:" << sslConfiguration.sessionProtocol();

	if (!checkAndSaveCertificate(sslConfiguration.peerCertificate()))
	{
		// checkAndSaveCertificate already set the error
		mReply->abort();
	}
}


void StateGenericSendReceive::onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
{
	qCDebug(network) << "pre-shared key authentication requested:" << pAuthenticator->identityHint();

	auto token = getContext()->getTcToken();
	Q_ASSERT(!token.isNull());
	if (!token->getPsk().isNull())
	{
		pAuthenticator->setIdentity(token->getSessionIdentifier());
		pAuthenticator->setPreSharedKey(QByteArray::fromHex(token->getPsk()));
	}
}


bool StateGenericSendReceive::checkAndSaveCertificate(const QSslCertificate& pCertificate)
{
	switch (CertificateChecker::checkAndSaveCertificate(pCertificate, getContext()->getTcToken()->getServerAddress(), getContext()))
	{
		case CertificateChecker::CertificateStatus::Good:
			return true;

		case CertificateChecker::CertificateStatus::Unsupported_Algorithm_Or_Length:
			reportChannelEstablishmentError(GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length);
			return false;

		case CertificateChecker::CertificateStatus::Hash_Not_In_Description:
			reportChannelEstablishmentError(GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description);
			return false;
	}

	return false;
}


void StateGenericSendReceive::run()
{
	QSharedPointer<PaosCreator> messageToSend = getMessageToSend();
	Q_ASSERT(!messageToSend.isNull());

	setMessageId(messageToSend.dynamicCast<PaosMessage>());
	if (QSharedPointer<ResponseType> paosResponse = messageToSend.dynamicCast<ResponseType>())
	{
		if (!getContext()->isErrorReportedToServer() && getContext()->getStatus().isError() && !getContext()->getStatus().isOriginServer())
		{
			paosResponse->setResult(getContext()->getStatus());
			getContext()->setErrorReportedToServer(true);
		}
		else
		{
			paosResponse->setResult(Result::createOk());
		}
	}

	Q_ASSERT(mReply.isNull());
	auto token = getContext()->getTcToken();
	Q_ASSERT(!token.isNull());

	QNetworkRequest request(token->getServerAddress());
	request.setRawHeader("requestid", token->getSessionIdentifier());
	QByteArray data = messageToSend->marshall();
	qCDebug(network) << "Try to send raw data:\n" << data;
	mReply = getContext()->getNetworkManager().paos(request, data, !token->getPsk().isNull());
	mConnections += connect(mReply.data(), &QNetworkReply::sslErrors, this, &StateGenericSendReceive::onSslErrors);
	mConnections += connect(mReply.data(), &QNetworkReply::encrypted, this, &StateGenericSendReceive::onSslHandshakeDone);
	mConnections += connect(mReply.data(), &QNetworkReply::finished, this, &StateGenericSendReceive::onReplyFinished);
	mConnections += connect(mReply.data(), &QNetworkReply::preSharedKeyAuthenticationRequired, this, &StateGenericSendReceive::onPreSharedKeyAuthenticationRequired);
}


void StateGenericSendReceive::onReplyFinished()
{
	qCDebug(network) << "Received message from eID-Server";
	QNetworkReply* reply = mReply.data();
	mReply.clear();
	reply->deleteLater();

	if (reply->error() != QNetworkReply::NoError)
	{
		reportChannelEstablishmentError(NetworkManager::toTrustedChannelStatus(reply));
		return;
	}

	for (const auto& header : reply->rawHeaderPairs())
	{
		qCDebug(network) << "RawHeader |" << header.first << "=" << header.second;
	}
	const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode >= 500)
	{
		reportChannelEstablishmentError(GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server);
		return;
	}

	QByteArray message = reply->readAll();
	qCDebug(network) << "Received raw data:\n" << message;
	PaosHandler paosHandler(message);
	qCDebug(network) << "Received PAOS message of type:" << paosHandler.getDetectedPaosType();

	if (!mTypesToReceive.contains(paosHandler.getDetectedPaosType()))
	{
		QString warnMsg("Received PAOS message not of expected type: ");
		for (const auto expectedType : mTypesToReceive)
		{
			warnMsg += expectedType;
			warnMsg += QLatin1Char(' ');
		}
		qCCritical(network) << warnMsg;

		if (paosHandler.getDetectedPaosType() == PaosType::UNKNOWN)
		{
			qCCritical(network) << "The program received an unknown message from the server.";
			setStatus(GlobalStatus::Code::Workflow_Unknown_Paos_Form_EidServer);
			Q_EMIT fireError();
		}
		else
		{
			qCCritical(network) << "The program received an unexpected message from the server.";
			setStatus(GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer);
			Q_EMIT fireError();
		}
		return;
	}

	setRemoteMessageId(paosHandler.getPaosMessage());
	setReceivedMessage(paosHandler.getPaosMessage());

	int result = mTypesToReceive.indexOf(paosHandler.getDetectedPaosType());
	if (result != 0)
	{
		/*
		 * the next states in the state machine are ordered as follows:
		 * possibleNextState0, error, shutdown, possibleNextState1, ... possibleNextState_n
		 *
		 * We therefore have to consider two cases here:
		 * 1) The detected PAOS type is known, so result >= 1
		 *    Then we add 2 in order to jump over the error state and the shutdown state
		 * 2) The detected PAOS type is unknown, so result == -1
		 *    By adding 2, result == 1 and the next state is state error
		 */

		result += 2;

		if (result == 1)
		{
			Q_EMIT fireError();
		}
		else if (result == 2)
		{
			Q_EMIT fireCancel();
		}
		else
		{
			emitStateMachineSignal(result);
		}
	}
	else
	{
		Q_EMIT fireSuccess();
	}
}
