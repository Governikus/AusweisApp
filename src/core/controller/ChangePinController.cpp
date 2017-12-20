/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinController.h"

#include "context/ChangePinContext.h"
#include "states/CompositeStateSelectCard.h"
#include "states/FinalState.h"
#include "states/StateChangePin.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateDestroyPace.h"
#include "states/StateEstablishPaceCan.h"
#include "states/StateEstablishPacePin.h"
#include "states/StateEstablishPacePuk.h"
#include "states/StateHandleRetryCounter.h"
#include "states/StateUpdateRetryCounter.h"

#include <QDebug>
#include <QFinalState>


using namespace governikus;


ChangePinController::ChangePinController(QSharedPointer<ChangePinContext> pContext)
	: WorkflowController(pContext)
{
	auto sSelectCard = new CompositeStateSelectCard(pContext);
	mStateMachine.addState(sSelectCard);
	auto sUpdateRetryCounter = addState<StateUpdateRetryCounter>();
	auto sHandleRetryCounter = addState<StateHandleRetryCounter>();
	auto sEstablishPacePin = addState<StateEstablishPacePin>();
	auto sEstablishPaceCan = addState<StateEstablishPaceCan>();
	auto sEstablishPacePuk = addState<StateEstablishPacePuk>();
	auto sChangePin = addState<StateChangePin>();
	auto sDestroyPace = addState<StateDestroyPace>();
	auto sUpdateRetryCounterFinal = addState<StateUpdateRetryCounter>();
	auto sCleanUpReaderManager = addState<StateCleanUpReaderManager>();

	auto sFinal = addState<FinalState>();
	mStateMachine.setInitialState(sSelectCard);

	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireContinue, sUpdateRetryCounter);
	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireAbort, sCleanUpReaderManager);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireContinue, sHandleRetryCounter);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireAbort, sCleanUpReaderManager);

	sHandleRetryCounter->addTransition(sHandleRetryCounter, &StateHandleRetryCounter::fireRetryCounterIsZero, sEstablishPacePuk);
	sHandleRetryCounter->addTransition(sHandleRetryCounter, &StateHandleRetryCounter::fireRetryCounterIsOne, sEstablishPaceCan);
	sHandleRetryCounter->addTransition(sHandleRetryCounter, &StateHandleRetryCounter::fireRetryCounterIsGTOne, sEstablishPacePin);
	sHandleRetryCounter->addTransition(sHandleRetryCounter, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sEstablishPacePuk->addTransition(sEstablishPacePuk, &StateEstablishPacePuk::fireInvalidPuk, sUpdateRetryCounter);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &StateEstablishPacePuk::fireInoperativePuk, sUpdateRetryCounterFinal);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireContinue, sUpdateRetryCounterFinal);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sEstablishPaceCan->addTransition(sEstablishPaceCan, &StateEstablishPaceCan::fireInvalidCan, sUpdateRetryCounter);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireContinue, sEstablishPacePin);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sEstablishPacePin->addTransition(sEstablishPacePin, &StateEstablishPacePin::fireInvalidPin, sUpdateRetryCounter);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireContinue, sChangePin);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sChangePin->addTransition(sChangePin, &AbstractState::fireContinue, sDestroyPace);
	sChangePin->addTransition(sChangePin, &StateChangePin::fireInvalidPin, sUpdateRetryCounterFinal);
	sChangePin->addTransition(sChangePin, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireContinue, sUpdateRetryCounterFinal);
	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sUpdateRetryCounterFinal->addTransition(sUpdateRetryCounterFinal, &AbstractState::fireContinue, sCleanUpReaderManager);
	sUpdateRetryCounterFinal->addTransition(sUpdateRetryCounterFinal, &AbstractState::fireAbort, sCleanUpReaderManager);

	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireContinue, sFinal);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireAbort, sFinal);
}


ChangePinController::~ChangePinController()
{
}
