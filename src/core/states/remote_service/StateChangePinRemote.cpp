/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangePinRemote.h"

#include "PinModify.h"
#include "ServerMessageHandler.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


StateChangePinRemote::StateChangePinRemote(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
	, mModifyPinMessage()
	, mMessageHandler()
{
}


StateChangePinRemote::~StateChangePinRemote()
{
}


void StateChangePinRemote::onEntry(QEvent* pEvent)
{
	AbstractGenericState::onEntry(pEvent);

	const QSharedPointer<RemoteServiceContext>& context = getContext();

	mModifyPinMessage = context->getModifyPinMessage();
	Q_ASSERT(mModifyPinMessage);

	const QSharedPointer<RemoteServer> remoteServer = context->getRemoteServer();
	Q_ASSERT(remoteServer);
	mMessageHandler = remoteServer->getMessageHandler();
	Q_ASSERT(mMessageHandler);

	auto cardConnection = context->getCardConnection();
	if (cardConnection.isNull())
	{
		Q_EMIT fireContinue();
		return;
	}

	mConnections += connect(context.data(), &RemoteServiceContext::fireCancelPasswordRequest, this, &StateChangePinRemote::onCancelChangePin);
	mConnections += connect(cardConnection.data(), &CardConnection::fireReaderInfoChanged, this, &StateChangePinRemote::onReaderInfoChanged);
	mConnections += connect(remoteServer.data(), &RemoteServer::fireConnectedChanged, this, &AbstractState::fireContinue);
}


void StateChangePinRemote::onExit(QEvent* pEvent)
{
	AbstractGenericState::onExit(pEvent);

	mMessageHandler.reset();
}


void StateChangePinRemote::run()
{
	const QSharedPointer<RemoteServiceContext>& context = getContext();

	PinModify pinModify(mModifyPinMessage->getInputData());
	const QString newPin = context->getNewPin();
	context->setPin(QString());
	const quint8 timeoutSeconds = pinModify.getTimeoutSeconds();

	auto cardConnection = context->getCardConnection();
	Q_ASSERT(cardConnection);
	mConnections += cardConnection->callSetEidPinCommand(this,
			&StateChangePinRemote::onChangePinDone,
			newPin,
			timeoutSeconds);
}


void StateChangePinRemote::onCancelChangePin()
{
	mMessageHandler->sendModifyPinResponse(mModifyPinMessage->getSlotHandle(), ResponseApdu(StatusCode::INPUT_CANCELLED));
	Q_EMIT fireContinue();
}


void StateChangePinRemote::onReaderInfoChanged(const ReaderInfo& pReaderInfo)
{
	if (!pReaderInfo.hasEidCard())
	{
		mMessageHandler->sendModifyPinResponse(mModifyPinMessage->getSlotHandle(), ResponseApdu(StatusCode::EMPTY));
		Q_EMIT fireContinue();
	}
}


void StateChangePinRemote::onChangePinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<SetEidPinCommand> command = pCommand.dynamicCast<SetEidPinCommand>();
	if (command)
	{
		mMessageHandler->sendModifyPinResponse(mModifyPinMessage->getSlotHandle(), command->getResponseApdu());
	}
	else
	{
		Q_ASSERT(false);
		qCDebug(remote_device) << "Expected a SetEidPinCommand as response!";
		mMessageHandler->sendModifyPinResponse(mModifyPinMessage->getSlotHandle(), ResponseApdu());
	}

	Q_EMIT fireContinue();
}
