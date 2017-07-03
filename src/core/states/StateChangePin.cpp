/*!
 * StateChangePin.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderManager.h"
#include "StateChangePin.h"

#include <QMetaObject>

using namespace governikus;

StateChangePin::StateChangePin(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateChangePin::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Invoke set Eid PIN command";
	mConnections += cardConnection->callSetEidPinCommand(this, &StateChangePin::onSetEidPinDone, getContext()->getNewPin());
}


void StateChangePin::onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const CardReturnCode returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case CardReturnCode::OK:
			getContext()->setSuccessMessage(tr("You have successfully changed your PIN."));
			Q_EMIT fireSuccess();
			break;

		case CardReturnCode::CANCELLATION_BY_USER:
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireCancel();
			break;

		case CardReturnCode::NEW_PIN_MISMATCH:
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireInvalidPin();
			break;

		default:
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireError();
			break;
	}
}
