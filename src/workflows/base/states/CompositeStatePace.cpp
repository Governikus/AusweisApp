/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "CompositeStatePace.h"

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"
#include "states/StateCheckError.h"
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
	auto* sMaintainCardConnection = StateBuilder::createState<StateMaintainCardConnection>(mContext);
	auto* sSelectReader = StateBuilder::createState<StateSelectReader>(mContext);
	auto* sConnectCard = StateBuilder::createState<StateConnectCard>(mContext);
	auto* sUpdateRetryCounter = StateBuilder::createState<StateUpdateRetryCounter>(mContext);
	auto* sVerifyRetryCounter = StateBuilder::createState<StateVerifyRetryCounter>(mContext);
	auto* sPreparePace = StateBuilder::createState<StatePreparePace>(mContext);
	auto* sEnterPacePassword = StateBuilder::createState<StateEnterPacePassword>(mContext);
	auto* sEstablishPaceChannel = StateBuilder::createState<StateEstablishPaceChannel>(mContext);
	auto* sUnfortunateCardPosition = StateBuilder::createState<StateUnfortunateCardPosition>(mContext);
	auto* sResetRetryCounter = StateBuilder::createState<StateResetRetryCounter>(mContext);
	auto* sDestroyPace = StateBuilder::createState<StateDestroyPace>(mContext);
	auto* sClearPacePasswords = StateBuilder::createState<StateClearPacePasswords>(mContext);
	auto* sCheckError = StateBuilder::createState<StateCheckError>(mContext);

	sMaintainCardConnection->setParent(this);
	sSelectReader->setParent(this);
	sConnectCard->setParent(this);
	sUpdateRetryCounter->setParent(this);
	sVerifyRetryCounter->setParent(this);
	sPreparePace->setParent(this);
	sEnterPacePassword->setParent(this);
	sEstablishPaceChannel->setParent(this);
	sUnfortunateCardPosition->setParent(this);
	sResetRetryCounter->setParent(this);
	sDestroyPace->setParent(this);
	sClearPacePasswords->setParent(this);
	sCheckError->setParent(this);

	setInitialState(sMaintainCardConnection);

	sMaintainCardConnection->addTransition(sMaintainCardConnection, &AbstractState::fireContinue, sVerifyRetryCounter);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &AbstractState::fireAbort, sClearPacePasswords);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireNoCardConnection, sSelectReader);
	sMaintainCardConnection->addTransition(sMaintainCardConnection, &StateMaintainCardConnection::fireForceUpdateRetryCounter, sUpdateRetryCounter);

	sSelectReader->addTransition(sSelectReader, &AbstractState::fireContinue, sConnectCard);
	sSelectReader->addTransition(sSelectReader, &AbstractState::fireAbort, sClearPacePasswords);
	sSelectReader->addTransition(sSelectReader, &StateSelectReader::fireRetry, sSelectReader);

	sConnectCard->addTransition(sConnectCard, &AbstractState::fireContinue, sUpdateRetryCounter);
	sConnectCard->addTransition(sConnectCard, &AbstractState::fireAbort, sClearPacePasswords);
	sConnectCard->addTransition(sConnectCard, &StateConnectCard::fireRetry, sSelectReader);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireContinue, sVerifyRetryCounter);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireAbort, sClearPacePasswords);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireNoCardConnection, sMaintainCardConnection);

	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireContinue, sPreparePace);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireAbort, sClearPacePasswords);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &StateVerifyRetryCounter::fireNoCardConnection, sMaintainCardConnection);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &StateVerifyRetryCounter::fireReaderOrCardChanged, sClearPacePasswords);

	sPreparePace->addTransition(sPreparePace, &AbstractState::fireContinue, sEstablishPaceChannel);
	sPreparePace->addTransition(sPreparePace, &AbstractState::fireAbort, sClearPacePasswords);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireNoCardConnection, sMaintainCardConnection);
	sPreparePace->addTransition(sPreparePace, &StatePreparePace::fireEnterPacePassword, sEnterPacePassword);

	sEnterPacePassword->addTransition(sEnterPacePassword, &AbstractState::fireContinue, sEstablishPaceChannel);
	sEnterPacePassword->addTransition(sEnterPacePassword, &AbstractState::fireAbort, sClearPacePasswords);

	connect(sEstablishPaceChannel, &AbstractState::fireContinue, this, &CompositeStatePace::fireContinue);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::firePaceCanEstablished, sMaintainCardConnection);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::firePacePukEstablished, sResetRetryCounter);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireAbort, sClearPacePasswords);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireNoCardConnection, sMaintainCardConnection);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::firePaceChannelFailed, sMaintainCardConnection);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireWrongPassword, sClearPacePasswords);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireThirdPinAttemptFailed, sDestroyPace);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &StateEstablishPaceChannel::fireAbortAndUnfortunateCardPosition, sUnfortunateCardPosition);

	sUnfortunateCardPosition->addTransition(sUnfortunateCardPosition, &AbstractState::fireContinue, sMaintainCardConnection);
	sUnfortunateCardPosition->addTransition(sUnfortunateCardPosition, &AbstractState::fireAbort, sClearPacePasswords);

	sResetRetryCounter->addTransition(sResetRetryCounter, &AbstractState::fireAbort, sDestroyPace);
	sResetRetryCounter->addTransition(sResetRetryCounter, &AbstractState::fireContinue, sDestroyPace);
	sResetRetryCounter->addTransition(sResetRetryCounter, &StateResetRetryCounter::fireNoCardConnection, sMaintainCardConnection);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireContinue, sClearPacePasswords);
	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireAbort, sClearPacePasswords);

	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireContinue, sCheckError);
	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireAbort, sCheckError);

	sCheckError->addTransition(sCheckError, &AbstractState::fireContinue, sMaintainCardConnection);
	connect(sCheckError, &AbstractState::fireAbort, this, &CompositeStatePace::fireAbort);
	connect(sCheckError, &StateCheckError::firePropagateAbort, this, &CompositeStatePace::firePropagateAbort);

}
