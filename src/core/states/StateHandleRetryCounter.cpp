/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "StateHandleRetryCounter.h"

#include "CardConnection.h"


using namespace governikus;


StateHandleRetryCounter::StateHandleRetryCounter(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateHandleRetryCounter::run()
{
	auto cardConnection = getContext()->getCardConnection();
	Q_ASSERT(cardConnection != nullptr);

	switch (cardConnection->getReaderInfo().getRetryCounter())
	{
		case 0:
			qDebug() << "PUK required";
			Q_EMIT fireRetryCounterIsZero();
			break;

		case 1:
			qDebug() << "CAN required";
			Q_EMIT fireRetryCounterIsOne();
			break;

		default:
			qDebug() << "PIN allowed";
			Q_EMIT fireRetryCounterIsGTOne();
	}

}
