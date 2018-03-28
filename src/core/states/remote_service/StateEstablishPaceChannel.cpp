/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateEstablishPaceChannel.h"

#include "EstablishPACEChannelParser.h"
#include "ServerMessageHandler.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


StateEstablishPaceChannel::StateEstablishPaceChannel(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


StateEstablishPaceChannel::~StateEstablishPaceChannel()
{
}


void StateEstablishPaceChannel::onEntry(QEvent* pEvent)
{
	AbstractGenericState::onEntry(pEvent);

	const QSharedPointer<RemoteServiceContext>& context = getContext();

	mEstablishPaceChannelMessage = context->getEstablishPaceChannelMessage();
	Q_ASSERT(mEstablishPaceChannelMessage);

	const QSharedPointer<RemoteServer> remoteServer = context->getRemoteServer();
	Q_ASSERT(remoteServer);
	mMessageHandler = remoteServer->getMessageHandler();
	Q_ASSERT(mMessageHandler);

	auto cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		Q_EMIT fireContinue();
		return;
	}

	mConnections += connect(cardConnection.data(), &CardConnection::fireReaderInfoChanged, this, &StateEstablishPaceChannel::onReaderInfoChanged);
	mConnections += connect(getContext().data(), &RemoteServiceContext::fireCancelPasswordRequest, this, &StateEstablishPaceChannel::onCancelEstablishPaceChannel);
	mConnections += connect(getContext()->getRemoteServer().data(), &RemoteServer::fireConnectedChanged, this, &AbstractState::fireContinue);
}


void StateEstablishPaceChannel::onExit(QEvent* pEvent)
{
	AbstractGenericState::onExit(pEvent);

	mMessageHandler.reset();
}


void StateEstablishPaceChannel::run()
{
	const QSharedPointer<RemoteServiceContext>& context = getContext();

	EstablishPACEChannelParser parser = EstablishPACEChannelParser::fromCcid(mEstablishPaceChannelMessage->getInputData());
	QString pacePassword;
	switch (parser.getPasswordId())
	{
		case PACE_PASSWORD_ID::PACE_CAN:
			pacePassword = context->getCan();
			context->setCan(QString());
			break;

		case PACE_PASSWORD_ID::PACE_PIN:
			pacePassword = context->getPin();
			context->setPin(QString());
			break;

		case PACE_PASSWORD_ID::PACE_PUK:
			pacePassword = context->getPuk();
			context->setPuk(QString());
			break;

		default:
			mMessageHandler->sendEstablishPaceChannelResponse(mEstablishPaceChannelMessage->getSlotHandle(), EstablishPACEChannelOutput());
			return;
	}

	auto cardConnection = context->getCardConnection();
	Q_ASSERT(cardConnection);
	mConnections += cardConnection->callEstablishPaceChannelCommand(this,
			&StateEstablishPaceChannel::onEstablishConnectionDone,
			parser.getPasswordId(),
			pacePassword,
			parser.getChat(),
			parser.getCertificateDescription());
}


void StateEstablishPaceChannel::onCancelEstablishPaceChannel()
{
	EstablishPACEChannelOutput channelOutput = EstablishPACEChannelOutput();
	channelOutput.setPaceReturnCode(CardReturnCode::CANCELLATION_BY_USER);
	mMessageHandler->sendEstablishPaceChannelResponse(mEstablishPaceChannelMessage->getSlotHandle(), channelOutput);

	Q_EMIT fireContinue();
}


void StateEstablishPaceChannel::onReaderInfoChanged(const ReaderInfo& pReaderInfo)
{
	if (!pReaderInfo.hasEidCard())
	{
		EstablishPACEChannelOutput channelOutput = EstablishPACEChannelOutput();
		channelOutput.setPaceReturnCode(CardReturnCode::CARD_NOT_FOUND);
		mMessageHandler->sendEstablishPaceChannelResponse(mEstablishPaceChannelMessage->getSlotHandle(), channelOutput);

		Q_EMIT fireContinue();
	}
}


void StateEstablishPaceChannel::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<EstablishPaceChannelCommand> establishPaceChannelCommand = pCommand.dynamicCast<EstablishPaceChannelCommand>();
	if (establishPaceChannelCommand)
	{
		mMessageHandler->sendEstablishPaceChannelResponse(mEstablishPaceChannelMessage->getSlotHandle(), establishPaceChannelCommand->getPaceOutput());
	}
	else
	{
		Q_ASSERT(false);
		qCDebug(remote_device) << "Expected an EstablishPaceChannelCommand as response!";
		mMessageHandler->sendEstablishPaceChannelResponse(mEstablishPaceChannelMessage->getSlotHandle(), EstablishPACEChannelOutput());
	}

	Q_EMIT fireContinue();
}
