/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CompositeStatePace.h"

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"
#include "states/StateClearPacePasswords.h"
#include "states/StateConnectCard.h"
#include "states/StateDestroyPace.h"
#include "states/StateEnterPacePassword.h"
#include "states/StateEstablishPaceChannel.h"
#include "states/StateMaintainCardConnection.h"
#include "states/StatePreparePace.h"
#include "states/StateSelectReader.h"
#include "states/StateUnfortunateCardPosition.h"
#include "states/StateUpdateRetryCounter.h"
#include "states/StateVerifyRetryCounter.h"

using namespace governikus;


CompositeStatePace::CompositeStatePace(const QSharedPointer<WorkflowContext>& pContext)
	: QState()
	, mContext(pContext)
{
	auto sMaintainCardConnection = StateBuilder::createState<StateMaintainCardConnection>(mContext);
	auto sClearPacePasswordsOnError = StateBuilder::createState<StateClearPacePasswords>(mContext);
	auto sSelectReader = StateBuilder::createState<StateSelectReader>(mContext);
	auto sConnectCard = StateBuilder::createState<StateConnectCard>(mContext);
	auto sUpdateRetryCounter = StateBuilder::createState<StateUpdateRetryCounter>(mContext);
	auto sVerifyRetryCounter = StateBuilder::createState<StateVerifyRetryCounter>(mContext);
	auto sPreparePace = StateBuilder::createState<StatePreparePace>(mContext);
	auto sUnfortunateCardPosition = StateBuilder::createState<StateUnfortunateCardPosition>(mContext);
	auto sEnterPacePassword = StateBuilder::createState<StateEnterPacePassword>(mContext);
	auto sEstablishPaceChannel = StateBuilder::createState<StateEstablishPaceChannel>(mContext);
	auto sClearPacePasswordsBeforeDestroy = StateBuilder::createState<StateClearPacePasswords>(mContext);
	auto sDestroyPace = StateBuilder::createState<StateDestroyPace>(mContext);

	sMaintainCardConnection->setParent(this);
	sClearPacePasswordsOnError->setParent(this);
	sSelectReader->setParent(this);
	sConnectCard->setParent(this);
	sUpdateRetryCounter->setParent(this);
	sVerifyRetryCounter->setParent(this);
	sPreparePace->setParent(this);
	sUnfortunateCardPosition->setParent(this);
	sEnterPacePassword->setParent(this);
	sEstablishPaceChannel->setParent(this);
	sClearPacePasswordsBeforeDestroy->setParent(this);
	sDestroyPace->setParent(this);

	setInitialState(sMaintainCardConnection);

	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireContinue, sVerifyRetryCounter);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireNoCardConnection, sSelectReader);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireForceUpdateRetryCounter, sUpdateRetryCounter);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireAbort, sClearPacePasswordsOnError);

	connect(sClearPacePasswordsOnError, &AbstractState::fireContinue, this, &CompositeStatePace::fireAbort);
	connect(sClearPacePasswordsOnError, &AbstractState::fireAbort, this, &CompositeStatePace::fireAbort);

	sSelectReader->addTransition(sSelectReader, &StateSelectReader::fireRetry, sSelectReader);
	sSelectReader->addTransition(sSelectReader, &AbstractState::fireContinue, sConnectCard);
	sSelectReader->addTransition(sSelectReader, &AbstractState::fireAbort, sMaintainCardConnection);

	sConnectCard->addTransition(sConnectCard, &StateConnectCard::fireRetry, sSelectReader);
	sConnectCard->addTransition(sConnectCard, &AbstractState::fireContinue, sUpdateRetryCounter);
	sConnectCard->addTransition(sConnectCard, &AbstractState::fireAbort, sMaintainCardConnection);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireContinue, sVerifyRetryCounter);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireAbort, sMaintainCardConnection);

	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireContinue, sPreparePace);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireAbort, sMaintainCardConnection);

	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireEnterPacePassword, sEnterPacePassword);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireEstablishPaceChannel, sEstablishPaceChannel);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireContinue, sMaintainCardConnection);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireAbort, sMaintainCardConnection);

	sEnterPacePassword->addTransition(sEnterPacePassword, &AbstractState::fireContinue, sEstablishPaceChannel);
	sEnterPacePassword->addTransition(sEnterPacePassword, &AbstractState::fireAbort, sMaintainCardConnection);

	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireContinue, sMaintainCardConnection);
	connect(sEstablishPaceChannel, &StateEstablishPaceChannel::firePaceChannelEstablished, this, &CompositeStatePace::firePaceChannelEstablished);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::firePaceChannelInoperative, sClearPacePasswordsBeforeDestroy);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireAbortAndUnfortunateCardPosition, sUnfortunateCardPosition);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireAbort, sMaintainCardConnection);

	sUnfortunateCardPosition->addTransition(sUnfortunateCardPosition, &AbstractState::fireContinue, sMaintainCardConnection);
	sUnfortunateCardPosition->addTransition(sUnfortunateCardPosition, &AbstractState::fireAbort, sMaintainCardConnection);

	sClearPacePasswordsBeforeDestroy->addTransition(sClearPacePasswordsBeforeDestroy, &AbstractState::fireContinue, sDestroyPace);
	sClearPacePasswordsBeforeDestroy->addTransition(sClearPacePasswordsBeforeDestroy, &AbstractState::fireAbort, sDestroyPace);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireContinue, sMaintainCardConnection);
	connect(sDestroyPace, &AbstractState::fireAbort, this, &CompositeStatePace::fireAbort);
}
