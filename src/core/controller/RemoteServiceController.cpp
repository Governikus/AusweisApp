/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "controller/RemoteServiceController.h"

#include "context/RemoteServiceContext.h"
#include "states/FinalState.h"
#include "states/remote_service/StateChangePinRemote.h"
#include "states/remote_service/StateEstablishPaceChannel.h"
#include "states/remote_service/StateProcessRemoteMessages.h"
#include "states/remote_service/StateStartRemoteService.h"
#include "states/remote_service/StateStopRemoteService.h"

#include <initializer_list>
#include <QDebug>


using namespace governikus;


RemoteServiceController::RemoteServiceController(QSharedPointer<RemoteServiceContext> pContext)
	: WorkflowController(pContext)
{
	auto sStartRemoteService = addState<StateStartRemoteService>();
	mStateMachine.setInitialState(sStartRemoteService);
	auto sProcessRemoteMessages = addState<StateProcessRemoteMessages>();
	auto sEstablishPaceChannel = addState<StateEstablishPaceChannel>();
	auto sChangePinRemote = addState<StateChangePinRemote>();
	auto sStopRemoteService = addState<StateStopRemoteService>();
	auto sFinal = addState<FinalState>();

	sStartRemoteService->addTransition(sStartRemoteService, &AbstractState::fireContinue, sProcessRemoteMessages);
	sStartRemoteService->addTransition(sStartRemoteService, &AbstractState::fireAbort, sStopRemoteService);

	sProcessRemoteMessages->addTransition(sProcessRemoteMessages, &AbstractState::fireAbort, sStopRemoteService);
	sProcessRemoteMessages->addTransition(sProcessRemoteMessages, &StateProcessRemoteMessages::fireEstablishPaceChannel, sEstablishPaceChannel);
	sProcessRemoteMessages->addTransition(sProcessRemoteMessages, &StateProcessRemoteMessages::fireModifyPin, sChangePinRemote);

	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireContinue, sProcessRemoteMessages);
	sEstablishPaceChannel->addTransition(sEstablishPaceChannel, &AbstractState::fireAbort, sStopRemoteService);

	sChangePinRemote->addTransition(sChangePinRemote, &AbstractState::fireContinue, sProcessRemoteMessages);
	sChangePinRemote->addTransition(sChangePinRemote, &AbstractState::fireAbort, sStopRemoteService);

	sStopRemoteService->addTransition(sStopRemoteService, &AbstractState::fireContinue, sFinal);
	sStopRemoteService->addTransition(sStopRemoteService, &AbstractState::fireAbort, sFinal);
}


RemoteServiceController::~RemoteServiceController()
{
}
