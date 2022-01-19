/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangePinRemote.h"

#include "PinModify.h"
#include "ServerMessageHandler.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


StateChangePinRemote::StateChangePinRemote(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateChangePinRemote::run()
{
	Q_ASSERT(getContext());
	Q_ASSERT(getContext()->getModifyPinMessage());

	const QSharedPointer<RemoteServiceContext>& context = getContext();
	auto cardConnection = context->getCardConnection();
	if (cardConnection.isNull())
	{
		context->setModifyPinMessageResponseApdu(ResponseApdu(StatusCode::EMPTY));
		Q_EMIT fireContinue();
		return;
	}

	QSharedPointer<const IfdModifyPin> modifyPinMessage = context->getModifyPinMessage();

	PinModify pinModify(modifyPinMessage->getInputData());
	const quint8 timeoutSeconds = pinModify.getTimeoutSeconds();

	Q_ASSERT(cardConnection);
	mConnections += cardConnection->callSetEidPinCommand(this,
			&StateChangePinRemote::onChangePinDone,
			context->getNewPin().toLatin1(),
			timeoutSeconds);
}


void StateChangePinRemote::onChangePinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<SetEidPinCommand> command = pCommand.objectCast<SetEidPinCommand>();
	if (command)
	{
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu(command->getResponseApdu()));
	}
	else
	{
		Q_ASSERT(false);
		qCDebug(remote_device) << "Expected a SetEidPinCommand as response!";
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu());
	}

	Q_EMIT fireContinue();
}
