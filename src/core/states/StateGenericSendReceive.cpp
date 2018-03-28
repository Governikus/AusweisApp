/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateGenericSendReceive.h"

#include "CertificateChecker.h"
#include "Env.h"
#include "paos/PaosHandler.h"
#include "TlsChecker.h"

#include <QTimer>

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


StateGenericSendReceive::StateGenericSendReceive(const QSharedPointer<WorkflowContext>& pContext, const QVector<PaosType>& pTypesToReceive)
	: AbstractGenericState(pContext)
	, mTypesToReceive(pTypesToReceive)
	, mReply()
{
}


StateGenericSendReceive::StateGenericSendReceive(const QSharedPointer<WorkflowContext>& pContext, PaosType pTypesToReceive)
	: StateGenericSendReceive(pContext, QVector<PaosType>() << pTypesToReceive)
{
}


void StateGenericSendReceive::setReceivedMessage(const QSharedPointer<PaosMessage>& pMessage)
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
			getContext()->setDisconnect(pMessage.staticCast<Disconnect>());
			getContext()->setDisconnectResponse(QSharedPointer<DisconnectResponse>(new DisconnectResponse()));
			break;

		default:
			qCWarning(network) << "Unknown received message type:" << static_cast<int>(pMessage->mType);
			break;
	}
}


void StateGenericSendReceive::setMessageId(const QSharedPointer<PaosMessage>& pPaosMessage)
{
	Q_ASSERT(!pPaosMessage.isNull());
	const QSharedPointer<MessageIdHandler> messageIdHandler = getContext()->getMessageIdHandler();
	pPaosMessage->setRelatesTo(messageIdHandler->getRemoteMsgId());
	pPaosMessage->setMessageId(messageIdHandler->createNewMsgId());
}


void StateGenericSendReceive::setRemoteMessageId(const QSharedPointer<PaosMessage>& pPaosMessage)
{
	const QSharedPointer<MessageIdHandler> messageIdHandler = getContext()->getMessageIdHandler();
	messageIdHandler->setRemoteMsgId(pPaosMessage->getMessageId());
}


void StateGenericSendReceive::onSslErrors(const QList<QSslError>& pErrors)
{
	if (TlsChecker::containsFatalError(mReply, pErrors))
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error);
		updateStatus(GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error);
		Q_EMIT fireAbort();
	}
}


void StateGenericSendReceive::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, qInfo(network));

	bool abort = false;
	const auto statusCode = checkAndSaveCertificate(cfg.peerCertificate());
	if (statusCode != GlobalStatus::Code::No_Error)
	{
		qCCritical(network) << GlobalStatus(statusCode);
		updateStatus(statusCode);
		abort = true;
	}

	auto context = getContext();
	if (!abort && !context->getTcToken()->usePsk())
	{
		const auto& session = context->getSslSession();
		if (session.isEmpty() || session != cfg.sessionTicket())
		{
			qCCritical(network) << "Session resumption failed";
			updateStatus(GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Establishment_Error);
			abort = true;
		}
	}

	if (abort)
	{
		// Stop listening to signals from QNetworkReply to stay in
		// this state until the following timer fires the signal.
		clearConnections();

		mReply->abort();

		// We need to append the abort to the event queue, because
		// QNetworkAccessManager::clearConnectionCache forces sending
		// the request when the abort on QNetworkReply is not finished.
		// See https://bugreports.qt.io/browse/QTBUG-65960
		QTimer::singleShot(0, this, [this] {
					Q_EMIT fireAbort();
				});
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


GlobalStatus::Code StateGenericSendReceive::checkAndSaveCertificate(const QSslCertificate& pCertificate)
{
	const QSharedPointer<AuthContext> c = getContext();
	Q_ASSERT(!c.isNull());

	std::function<void(const QUrl&, const QSslCertificate&)> saveCertificateFunc = [&c]
				(const QUrl& pUrl, const QSslCertificate& pCert)
			{
				c->addCertificateData(pUrl, pCert);
			};

	switch (CertificateChecker::checkAndSaveCertificate(pCertificate, c->getTcToken()->getServerAddress(), c->getDidAuthenticateEac1(), c->getDvCvc(), saveCertificateFunc))
	{
		case CertificateChecker::CertificateStatus::Good:
			return GlobalStatus::Code::No_Error;

		case CertificateChecker::CertificateStatus::Unsupported_Algorithm_Or_Length:
			return GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length;

		case CertificateChecker::CertificateStatus::Hash_Not_In_Description:
			return GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description;
	}

	return GlobalStatus::Code::Workflow_Processing_Error;
}


void StateGenericSendReceive::run()
{
	setMessageId(getAsMessage());

	if (QSharedPointer<ResponseType> paosResponse = getAsResponse())
	{
		if (!getContext()->isErrorReportedToServer() && getContext()->getStatus().isError() && !getContext()->getStatus().isOriginServer())
		{
			if (getContext()->getTransmitResponseFailed())
			{
				paosResponse->setResult(GlobalStatus(GlobalStatus::Code::Paos_Error_AL_Unknown_Error));
			}
			else
			{
				paosResponse->setResult(getContext()->getStatus());
			}
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
	const QByteArray& data = getAsCreator()->marshall();
	Q_ASSERT(!data.isEmpty());

	qCDebug(network) << "Try to send raw data:\n" << data;
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
	QNetworkReply* reply = mReply.data();
	mReply.clear();
	reply->deleteLater();

	if (reply->error() != QNetworkReply::NoError)
	{
		const auto& statusCode = NetworkManager::toTrustedChannelStatus(reply);
		qCCritical(network) << GlobalStatus(statusCode);
		updateStatus(statusCode);
		Q_EMIT fireAbort();
		return;
	}

	for (const auto& header : reply->rawHeaderPairs())
	{
		qCDebug(network) << "RawHeader |" << header.first << "=" << header.second;
	}
	const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	if (statusCode >= 500)
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server);
		updateStatus(GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server);
		Q_EMIT fireAbort();
		return;
	}

	QByteArray message = reply->readAll();
	qCDebug(network) << "Received raw data:\n" << message;
	PaosHandler paosHandler(message);
	qCDebug(network) << "Received PAOS message of type:" << paosHandler.getDetectedPaosType();

	if (!mTypesToReceive.contains(paosHandler.getDetectedPaosType()))
	{
		QString warnMsg = QStringLiteral("Received PAOS message not of expected type: ");
		for (const auto expectedType : mTypesToReceive)
		{
			warnMsg += expectedType;
			warnMsg += QLatin1Char(' ');
		}
		qCCritical(network) << warnMsg;

		if (paosHandler.getDetectedPaosType() == PaosType::UNKNOWN)
		{
			qCCritical(network) << "The program received an unknown message from the server.";
			updateStatus(GlobalStatus::Code::Workflow_Unknown_Paos_Form_EidServer);
			Q_EMIT fireAbort();
		}
		else
		{
			qCCritical(network) << "The program received an unexpected message from the server.";
			updateStatus(GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer);
			Q_EMIT fireAbort();
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
			Q_EMIT fireAbort();
		}
		else if (result == 2)
		{
			Q_EMIT fireAbort();
		}
		else
		{
			emitStateMachineSignal(result);
		}
	}
	else
	{
		Q_EMIT fireContinue();
	}
}
