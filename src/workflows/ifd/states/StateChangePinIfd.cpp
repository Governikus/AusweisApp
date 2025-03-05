/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateChangePinIfd.h"

#include "pinpad/PinModify.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)
Q_DECLARE_LOGGING_CATEGORY(statemachine)


using namespace governikus;


StateChangePinIfd::StateChangePinIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateChangePinIfd::run()
{
	Q_ASSERT(getContext());
	Q_ASSERT(getContext()->getModifyPinMessage());

	const QSharedPointer<IfdServiceContext>& context = getContext();
	auto cardConnection = context->getCardConnection();
	if (!cardConnection)
	{
		qCDebug(statemachine) << "No card connection available";
		context->setModifyPinMessageResponseApdu(ResponseApdu());
		Q_EMIT fireContinue();
		return;
	}

	QSharedPointer<const IfdModifyPin> modifyPinMessage = context->getModifyPinMessage();

	PinModify pinModify(modifyPinMessage->getInputData());
	const quint8 timeoutSeconds = pinModify.getTimeoutSeconds();

	*this << cardConnection->callSetEidPinCommand(this,
			&StateChangePinIfd::onChangePinDone,
			context->getNewPin().toLatin1(),
			timeoutSeconds);
}


void StateChangePinIfd::onChangePinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const QSharedPointer<SetEidPinCommand> command = pCommand.objectCast<SetEidPinCommand>();
	if (command)
	{
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu(command->getResponseApdu()));
	}
	else
	{
		Q_ASSERT(false);
		qCDebug(ifd) << "Expected a SetEidPinCommand as response!";
		getContext()->setModifyPinMessageResponseApdu(ResponseApdu());
	}

	Q_EMIT fireContinue();
}
