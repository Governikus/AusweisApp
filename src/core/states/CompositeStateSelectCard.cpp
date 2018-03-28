/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "CompositeStateSelectCard.h"

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"
#include "states/StateConnectCard.h"
#include "states/StateSelectReader.h"

using namespace governikus;


CompositeStateSelectCard::CompositeStateSelectCard(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto selectReader = StateBuilder::createState<StateSelectReader>(mContext);
	auto connectCard = StateBuilder::createState<StateConnectCard>(mContext);

	selectReader->setParent(this);
	connectCard->setParent(this);

	setInitialState(selectReader);

	selectReader->addTransition(selectReader, &StateSelectReader::fireRetry, selectReader);
	selectReader->addTransition(selectReader, &AbstractState::fireContinue, connectCard);
	connect(selectReader, &AbstractState::fireAbort, this, &CompositeStateSelectCard::fireAbort);

	connectCard->addTransition(connectCard, &StateConnectCard::fireRetry, selectReader);
	connectCard->addTransition(connectCard, &StateConnectCard::fireReaderRemoved, selectReader);
	connect(connectCard, &AbstractState::fireContinue, this, &CompositeStateSelectCard::fireContinue);
	connect(connectCard, &AbstractState::fireAbort, this, &CompositeStateSelectCard::fireAbort);
}


CompositeStateSelectCard::~CompositeStateSelectCard()
{
}
