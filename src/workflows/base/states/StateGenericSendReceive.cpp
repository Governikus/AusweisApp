/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateGenericSendReceive.h"

#include "AppSettings.h"
#include "CertificateChecker.h"
#include "Env.h"
#include "LogHandler.h"
#include "paos/PaosHandler.h"
#include "TlsChecker.h"

Q_DECLARE_LOGGING_CATEGORY(developermode)
Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;


StateGenericSendReceive::StateGenericSendReceive(const QSharedPointer<WorkflowContext>& pContext, const QVector<PaosType>& pTypesToReceive, bool pConnectOnCardRemoved)
	: AbstractState(pContext, pConnectOnCardRemoved)
	, GenericContextContainer(pContext)
	, mTypesToReceive(pTypesToReceive)
	, mReply()
{
}


StateGenericSendReceive::StateGenericSendReceive(const QSharedPointer<WorkflowContext>& pContext, PaosType pTypesToReceive)
	: StateGenericSendReceive(pContext, QVector<PaosType>
			{
				pTypesToReceive
			})
{
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

		case PaosType::DID_LIST:
			getContext()->setDidList(pMessage.staticCast<DIDList>());
			getContext()->setDidListResponse(QSharedPointer<DIDListResponse>::create());
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
			getContext()->addTransmit(pMessage.staticCast<Transmit>());
			getContext()->addTransmitResponse(QSharedPointer<TransmitResponse>::create());
			break;

		case PaosType::DISCONNECT:
			getContext()->setDisconnect(pMessage.staticCast<Disconnect>());
			getContext()->setDisconnectResponse(QSharedPointer<DisconnectResponse>::create());
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
		Q_EMIT fireAbort();
	}
}


void StateGenericSendReceive::onSslHandshakeDone()
{
	const auto& cfg = mReply->sslConfiguration();
	TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

	bool abort = false;
	const auto statusCode = checkAndSaveCertificate(cfg.peerCertificate());
	if (statusCode != GlobalStatus::Code::No_Error)
	{
		qCCritical(network) << GlobalStatus(statusCode);

		switch (statusCode)
		{
			case GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description:
			case GlobalStatus::Code::Workflow_Network_Ssl_Hash_Not_In_Certificate_Description:
			case GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
			case GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
			{
				const GlobalStatus::ExternalInfoMap infoMap {
					{GlobalStatus::ExternalInformation::CERTIFICATE_ISSUER_NAME, TlsChecker::getCertificateIssuerName(cfg.peerCertificate())},
					{GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				};
				updateStatus({statusCode, infoMap});
				break;
			}

			default:
				updateStatus({statusCode, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
						});
				break;
		}
		abort = true;
	}

	auto context = getContext();
	if (!abort && !context->getTcToken()->usePsk())
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
				abort = true;
			}
		}
	}

	if (abort)
	{
		// Stop listening to signals from QNetworkReply to stay in
		// this state until the following timer fires the signal.
		clearConnections();
		mReply->abort();
		Q_EMIT fireAbort();
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
	AbstractState::onExit(pEvent);

	if (!mReply.isNull())
	{
		mReply.reset();
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
	Q_ASSERT(getContext()->getPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	Q_ASSERT(getContext()->getCan().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	Q_ASSERT(getContext()->getPuk().isEmpty() && "PACE passwords must be cleared as soon as possible.");

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

	QNetworkRequest request(token->getServerAddress());
	request.setRawHeader("requestid", token->getSessionIdentifier());
	const QByteArray& data = getAsCreator()->marshall();
	Q_ASSERT(!data.isEmpty());

	qCDebug(network).noquote() << "Try to send raw data:\n" << data;
	const QByteArray& paosNamespace = PaosCreator::getNamespace(PaosCreator::Namespace::PAOS).toUtf8();
	const auto& session = token->usePsk() ? QByteArray() : getContext()->getSslSession();
	mReply.reset(Env::getSingleton<NetworkManager>()->paos(request, paosNamespace, data, token->usePsk(), session), &QObject::deleteLater);
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
		Q_EMIT fireAbort();
		return;
	}

	if (statusCode >= 500)
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server);
		updateStatus({GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		Q_EMIT fireAbort();
		return;
	}

	if (statusCode >= 400)
	{
		qCCritical(network) << GlobalStatus(GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer);
		updateStatus({GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
				});
		Q_EMIT fireAbort();
		return;
	}

	QByteArray message = mReply->readAll();
	qCDebug(network).noquote() << "Received raw data:\n" << message;
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
			updateStatus({GlobalStatus::Code::Workflow_Unknown_Paos_From_EidServer, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
					});
			Q_EMIT fireAbort();
		}
		else
		{
			qCCritical(network) << "The program received an unexpected message from the server.";
			updateStatus({GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer, {GlobalStatus::ExternalInformation::LAST_URL, mReply->url().toString()}
					});
			Q_EMIT fireAbort();
		}
		return;
	}

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
