/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"
#include "StateChangePin.h"

#include <QMetaObject>

using namespace governikus;

StateChangePin::StateChangePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateChangePin::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Invoke set Eid PIN command";
	mConnections += cardConnection->callSetEidPinCommand(this, &StateChangePin::onSetEidPinDone, getContext()->getNewPin().toLatin1());
}


void StateChangePin::onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const CardReturnCode returnCode = pCommand->getReturnCode();
	getContext()->setLastPaceResult(returnCode);
	switch (returnCode)
	{
		case CardReturnCode::OK:
			//: INFO ALL_PLATFORMS The pin was changed successfully.
			getContext()->setSuccessMessage(tr("You have successfully changed your PIN."));
			Q_EMIT fireContinue();
			break;

		case CardReturnCode::CANCELLATION_BY_USER:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireAbort();
			break;

		case CardReturnCode::NEW_PIN_MISMATCH:
			updateStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireInvalidPin();
			break;

		default:
			Q_EMIT fireAbort();
			break;
	}
}
