/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinController.h"

#include "context/ChangePinContext.h"
#include "states/CompositeStatePace.h"
#include "states/FinalState.h"
#include "states/StateChangePin.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateClearPacePasswords.h"
#include "states/StateDestroyPace.h"
#include "states/StateEnterNewPacePin.h"
#include "states/StatePrepareChangePin.h"
#include "states/StateUpdateRetryCounter.h"

#include <QDebug>
#include <QFinalState>


using namespace governikus;


ChangePinController::ChangePinController(QSharedPointer<ChangePinContext> pContext)
	: WorkflowController(pContext)
{
	auto sStatePace = new CompositeStatePace(pContext);
	mStateMachine.addState(sStatePace);
	auto sPrepareChangePin = addState<StatePrepareChangePin>();
	auto sEnterNewPacePin = addState<StateEnterNewPacePin>();
	auto sChangePin = addState<StateChangePin>();
	auto sClearPacePasswords = addState<StateClearPacePasswords>();
	auto sDestroyPace = addState<StateDestroyPace>();
	auto sUpdateRetryCounterFinal = addState<StateUpdateRetryCounter>();
	auto sCleanUpReaderManager = addState<StateCleanUpReaderManager>();

	auto sFinal = addState<FinalState>();
	mStateMachine.setInitialState(sStatePace);

	sStatePace->addTransition(sStatePace, &CompositeStatePace::fireContinue, sPrepareChangePin);
	sStatePace->addTransition(sStatePace, &CompositeStatePace::fireAbort, sClearPacePasswords);

	sPrepareChangePin->addTransition(sPrepareChangePin, &AbstractState::fireContinue, sChangePin);
	sPrepareChangePin->addTransition(sPrepareChangePin, &AbstractState::fireAbort, sDestroyPace);
	sPrepareChangePin->addTransition(sPrepareChangePin, &StatePrepareChangePin::fireEnterNewPacePin, sEnterNewPacePin);

	sEnterNewPacePin->addTransition(sEnterNewPacePin, &AbstractState::fireContinue, sChangePin);
	sEnterNewPacePin->addTransition(sEnterNewPacePin, &AbstractState::fireAbort, sDestroyPace);

	sChangePin->addTransition(sChangePin, &AbstractState::fireContinue, sDestroyPace);
	sChangePin->addTransition(sChangePin, &AbstractState::fireAbort, sDestroyPace);
	sChangePin->addTransition(sChangePin, &StateChangePin::fireRetry, sStatePace);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireContinue, sClearPacePasswords);
	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireAbort, sClearPacePasswords);

	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireContinue, sUpdateRetryCounterFinal);
	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireAbort, sUpdateRetryCounterFinal);

	sUpdateRetryCounterFinal->addTransition(sUpdateRetryCounterFinal, &AbstractState::fireContinue, sCleanUpReaderManager);
	sUpdateRetryCounterFinal->addTransition(sUpdateRetryCounterFinal, &AbstractState::fireAbort, sCleanUpReaderManager);

	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireContinue, sFinal);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireAbort, sFinal);
}


QSharedPointer<WorkflowRequest> ChangePinController::createWorkflowRequest(bool pRequestTransportPin)
{
	const auto& handler = [](const WorkflowRequest& pRequest, const QSharedPointer<WorkflowRequest>& pActiveWorkflow, const QSharedPointer<WorkflowRequest>& pWaitingWorkflow){
				Q_UNUSED(pRequest)
				Q_UNUSED(pActiveWorkflow)

				if (pWaitingWorkflow.isNull())
				{
					return WorkflowControl::ENQUEUE;
				}

				return WorkflowControl::SKIP;
			};

	return WorkflowRequest::createWorkflowRequestHandler<ChangePinController, ChangePinContext>(handler, pRequestTransportPin);
}
