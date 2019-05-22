/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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
	auto sSelectReader = StateBuilder::createState<StateSelectReader>(mContext);
	auto sConnectCard = StateBuilder::createState<StateConnectCard>(mContext);

	sSelectReader->setParent(this);
	sConnectCard->setParent(this);

	setInitialState(sSelectReader);

	sSelectReader->addTransition(sSelectReader, &StateSelectReader::fireRetry, sSelectReader);
	sSelectReader->addTransition(sSelectReader, &AbstractState::fireContinue, sConnectCard);
	connect(sSelectReader, &AbstractState::fireAbort, this, &CompositeStateSelectCard::fireAbort);

	sConnectCard->addTransition(sConnectCard, &StateConnectCard::fireRetry, sSelectReader);
	sConnectCard->addTransition(sConnectCard, &StateConnectCard::fireReaderRemoved, sSelectReader);
	connect(sConnectCard, &AbstractState::fireContinue, this, &CompositeStateSelectCard::fireContinue);
	connect(sConnectCard, &AbstractState::fireAbort, this, &CompositeStateSelectCard::fireAbort);
}


CompositeStateSelectCard::~CompositeStateSelectCard()
{
}
