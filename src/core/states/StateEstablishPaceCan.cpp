/*!
 * StateEstablishPaceCan.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "StateEstablishPaceCan.h"

#include "ReturnCodeUtil.h"


using namespace governikus;

StateEstablishPaceCan::StateEstablishPaceCan(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEstablishPaceCan::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Establish Pace connection with Can";
	mConnections += cardConnection->callEstablishPaceChannelCommand(this, &StateEstablishPaceCan::onEstablishConnectionDone, PACE_PIN_ID::PACE_CAN, getContext()->getCan());
	getContext()->setCan(QString());
}


void StateEstablishPaceCan::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(ReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPaceCan::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const ReturnCode returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case ReturnCode::OK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireSuccess();
			break;

		case ReturnCode::CANCELLATION_BY_USER:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			setResult(Result::createCancelByUserError());
			Q_EMIT fireCancel();
			break;

		case ReturnCode::CAN_INVALID:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidCan();
			break;

		default:
			setResult(Result::createInternalError(ReturnCodeUtil::toMessage(returnCode)));
			Q_EMIT fireError();
			break;
	}
}
