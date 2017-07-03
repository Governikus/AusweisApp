/*!
 * StateChangePin.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderManager.h"
#include "ReturnCodeUtil.h"
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
	qDebug() << "Invoke set Eid pin command";
	mConnections += cardConnection->callSetEidPinCommand(this, &StateChangePin::onSetEidPinDone, getContext()->getNewPin());
}


void StateChangePin::onSetEidPinDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const ReturnCode returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case ReturnCode::OK:
			getContext()->setSuccessMessage(tr("Pin successfully changed"));
			Q_EMIT fireSuccess();
			break;

		case ReturnCode::CANCELLATION_BY_USER:
			setResult(Result::createCancelByUserError());
			Q_EMIT fireCancel();
			break;

		case ReturnCode::NEW_PINS_DONT_MATCH:
			setResult(Result::createInternalError(ReturnCodeUtil::toMessage(returnCode)));
			Q_EMIT fireInvalidPin();
			break;

		default:
			setResult(Result::createInternalError(ReturnCodeUtil::toMessage(returnCode)));
			Q_EMIT fireError();
			break;
	}
}
