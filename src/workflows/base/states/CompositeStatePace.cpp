/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
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
#include "states/StateResetRetryCounter.h"
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
	auto sResetRetryCounter = StateBuilder::createState<StateResetRetryCounter>(mContext);
	auto sDestroyPace = StateBuilder::createState<StateDestroyPace>(mContext);
	auto sClearPacePasswords = StateBuilder::createState<StateClearPacePasswords>(mContext);

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
	sResetRetryCounter->setParent(this);
	sDestroyPace->setParent(this);
	sClearPacePasswords->setParent(this);

	setInitialState(sMaintainCardConnection);

	sMaintainCardConnection->addTransition(sMaintainCardConnection, &AbstractState::fireContinue, sVerifyRetryCounter);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &AbstractState::fireAbort, sClearPacePasswordsOnError);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::firePropagateAbort, sClearPacePasswordsOnError);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireNoCardConnection, sSelectReader);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireForceUpdateRetryCounter, sUpdateRetryCounter);

	connect(sClearPacePasswordsOnError, &AbstractState::fireContinue, this, &CompositeStatePace::firePropagateAbort);
	connect(sClearPacePasswordsOnError, &AbstractState::fireAbort, this, &CompositeStatePace::fireAbort);

	sSelectReader->addTransition(sSelectReader, &AbstractState::fireContinue, sConnectCard);
	sSelectReader->addTransition(sSelectReader, &AbstractState::fireAbort, sMaintainCardConnection);
	sSelectReader->addTransition(sSelectReader, &StateSelectReader::fireRetry, sSelectReader);

	sConnectCard->addTransition(sConnectCard, &AbstractState::fireContinue, sUpdateRetryCounter);
	sConnectCard->addTransition(sConnectCard, &AbstractState::fireAbort, sMaintainCardConnection);
	sConnectCard->addTransition(sConnectCard, &StateConnectCard::fireRetry, sSelectReader);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireContinue, sVerifyRetryCounter);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireAbort, sMaintainCardConnection);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireNoCardConnection, sMaintainCardConnection);

	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireContinue, sPreparePace);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireAbort, sMaintainCardConnection);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &StateVerifyRetryCounter::fireNoCardConnection, sMaintainCardConnection);

	sPreparePace->addTransition(sPreparePace, &AbstractState::fireContinue, sMaintainCardConnection);
	sPreparePace->addTransition(sPreparePace, &AbstractState::fireAbort, sMaintainCardConnection);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireNoCardConnection, sMaintainCardConnection);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireEnterPacePassword, sEnterPacePassword);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireEstablishPaceChannel, sEstablishPaceChannel);

	sEnterPacePassword->addTransition(sEnterPacePassword, &AbstractState::fireContinue, sEstablishPaceChannel);
	sEnterPacePassword->addTransition(sEnterPacePassword, &AbstractState::fireAbort, sMaintainCardConnection);
	sEnterPacePassword->addTransition(sEnterPacePassword, &StateEnterPacePassword::firePropagateAbort, sMaintainCardConnection);

	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireContinue, sMaintainCardConnection);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireNoCardConnection, sMaintainCardConnection);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireAbort, sMaintainCardConnection);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::firePaceChannelFailed, sMaintainCardConnection);
	connect(sEstablishPaceChannel, &StateEstablishPaceChannel::firePaceChannelEstablished, this, &CompositeStatePace::fireContinue);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireWrongPin, sClearPacePasswords);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireThirdPinAttemptFailed, sDestroyPace);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::firePacePukEstablished, sResetRetryCounter);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireAbortAndUnfortunateCardPosition, sUnfortunateCardPosition);

	sResetRetryCounter->addTransition(sResetRetryCounter, &AbstractState::fireAbort, sDestroyPace);
	sResetRetryCounter->addTransition(sResetRetryCounter, &AbstractState::fireContinue, sDestroyPace);
	sResetRetryCounter->addTransition(sResetRetryCounter, &StateResetRetryCounter::fireNoCardConnection, sMaintainCardConnection);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireContinue, sClearPacePasswords);
	connect(sDestroyPace, &AbstractState::fireAbort, this, &CompositeStatePace::fireAbort);

	sUnfortunateCardPosition->addTransition(sUnfortunateCardPosition, &AbstractState::fireContinue, sMaintainCardConnection);
	sUnfortunateCardPosition->addTransition(sUnfortunateCardPosition, &AbstractState::fireAbort, sMaintainCardConnection);

	connect(sClearPacePasswords, &AbstractState::fireAbort, this, &CompositeStatePace::fireAbort);
	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireContinue, sMaintainCardConnection);
}
