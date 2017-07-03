/*!
 * StateEstablishPaceCan.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "StateEstablishPaceCan.h"

using namespace governikus;

StateEstablishPaceCan::StateEstablishPaceCan(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEstablishPaceCan::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Establish Pace connection with CAN";
	mConnections += cardConnection->callEstablishPaceChannelCommand(this, &StateEstablishPaceCan::onEstablishConnectionDone, PACE_PIN_ID::PACE_CAN, getContext()->getCan());
	getContext()->setCan(QString());
}


void StateEstablishPaceCan::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(CardReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPaceCan::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	const CardReturnCode returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case CardReturnCode::OK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireSuccess();
			break;

		case CardReturnCode::CANCELLATION_BY_USER:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireCancel();
			break;

		case CardReturnCode::INVALID_CAN:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidCan();
			break;

		default:
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireError();
			break;
	}
}
