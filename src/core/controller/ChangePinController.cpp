/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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
#include "states/StateUpdateRetryCounter.h"


#include <QDebug>
#include <QFinalState>

using namespace governikus;

ChangePinController::ChangePinController(QSharedPointer<ChangePinContext> pContext)
	: WorkflowController(pContext)
{
	auto sSelectCard = new CompositeStateSelectCard(pContext);
	mStateMachine.addState(sSelectCard);
	auto sUpdateRetryCounter = addAndConnectState<StateUpdateRetryCounter>();
	auto sEstablishPacePin = addAndConnectState<StateEstablishPacePin>();
	auto sEstablishPaceCan = addAndConnectState<StateEstablishPaceCan>();
	auto sEstablishPacePuk = addAndConnectState<StateEstablishPacePuk>();
	auto sChangePin = addAndConnectState<StateChangePin>();
	auto sDestroyPace = addAndConnectState<StateDestroyPace>();
	auto sCleanUpReaderManager = addAndConnectState<StateCleanUpReaderManager>();

	auto sFinal = addAndConnectState<FinalState>();
	mStateMachine.setInitialState(sSelectCard);

	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireSuccess, sUpdateRetryCounter);
	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireError, sCleanUpReaderManager);
	sSelectCard->addTransition(sSelectCard, &CompositeStateSelectCard::fireCancel, sCleanUpReaderManager);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireRetryCounterIsZero, sEstablishPacePuk);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireRetryCounterIsOne, sEstablishPaceCan);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireRetryCounterIsGTOne, sEstablishPacePin);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireError, sCleanUpReaderManager);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireCancel, sCleanUpReaderManager);

	sEstablishPacePuk->addTransition(sEstablishPacePuk, &StateEstablishPacePuk::fireInvalidPuk, sUpdateRetryCounter);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &StateEstablishPacePuk::fireInoperativePuk, sUpdateRetryCounter);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireSuccess, sCleanUpReaderManager);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireError, sCleanUpReaderManager);
	sEstablishPacePuk->addTransition(sEstablishPacePuk, &AbstractState::fireCancel, sCleanUpReaderManager);

	sEstablishPaceCan->addTransition(sEstablishPaceCan, &StateEstablishPaceCan::fireInvalidCan, sUpdateRetryCounter);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireSuccess, sEstablishPacePin);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireError, sCleanUpReaderManager);
	sEstablishPaceCan->addTransition(sEstablishPaceCan, &AbstractState::fireCancel, sCleanUpReaderManager);

	sEstablishPacePin->addTransition(sEstablishPacePin, &StateEstablishPacePin::fireInvalidPin, sUpdateRetryCounter);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireSuccess, sChangePin);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireError, sCleanUpReaderManager);
	sEstablishPacePin->addTransition(sEstablishPacePin, &AbstractState::fireCancel, sCleanUpReaderManager);

	sChangePin->addTransition(sChangePin, &AbstractState::fireSuccess, sDestroyPace);
	sChangePin->addTransition(sChangePin, &StateChangePin::fireInvalidPin, sCleanUpReaderManager);
	sChangePin->addTransition(sChangePin, &AbstractState::fireError, sCleanUpReaderManager);
	sChangePin->addTransition(sChangePin, &AbstractState::fireCancel, sCleanUpReaderManager);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireSuccess, sCleanUpReaderManager);
	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireError, sCleanUpReaderManager);
	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireCancel, sCleanUpReaderManager);

	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireSuccess, sFinal);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireError, sFinal);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireCancel, sFinal);
}


ChangePinController::~ChangePinController()
{
}
