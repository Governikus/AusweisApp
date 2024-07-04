/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "IfdServiceController.h"

#include "context/IfdServiceContext.h"
#include "states/FinalState.h"
#include "states/StateChangePinIfd.h"
#include "states/StateChangePinResponse.h"
#include "states/StateClearPacePasswords.h"
#include "states/StateEnterNewPacePinIfd.h"
#include "states/StateEnterPacePasswordIfd.h"
#include "states/StateEstablishPaceChannelIfd.h"
#include "states/StateEstablishPaceChannelResponse.h"
#include "states/StatePrepareChangePinIfd.h"
#include "states/StatePreparePaceIfd.h"
#include "states/StateProcessIfdMessages.h"
#include "states/StateStartIfdService.h"
#include "states/StateStopIfdService.h"
#include "states/StateUpdateRetryCounter.h"
#include "states/StateVerifyRetryCounter.h"

#include <QDebug>


using namespace governikus;


IfdServiceController::IfdServiceController(QSharedPointer<IfdServiceContext> pContext)
	: WorkflowController(pContext)
{
	auto sStartIfdService = addInitialState<StateStartIfdService>();
	auto sProcessIfdMessages = addState<StateProcessIfdMessages>();
	auto sUpdateRetryCounter = addState<StateUpdateRetryCounter>();
	auto sVerifyRetryCounter = addState<StateVerifyRetryCounter>();
	auto sClearPacePasswordsOnNewCard = addState<StateClearPacePasswords>();
	auto sPreparePaceIfd = addState<StatePreparePaceIfd>();
	auto sEnterPacePasswordIfd = addState<StateEnterPacePasswordIfd>();
	auto sEstablishPaceChannelIfd = addState<StateEstablishPaceChannelIfd>();
	auto sEstablishPaceChannelResponse = addState<StateEstablishPaceChannelResponse>();
	auto sPrepareChangePinIfd = addState<StatePrepareChangePinIfd>();
	auto sEnterNewPacePinIfd = addState<StateEnterNewPacePinIfd>();
	auto sChangePinIfd = addState<StateChangePinIfd>();
	auto sChangePinResponse = addState<StateChangePinResponse>();
	auto sClearPacePasswords = addState<StateClearPacePasswords>();
	auto sClearPacePasswordsBeforeStop = addState<StateClearPacePasswords>();
	auto sStopIfdService = addState<StateStopIfdService>();
	auto sFinal = addState<FinalState>();

	sStartIfdService->addTransition(sStartIfdService, &AbstractState::fireContinue, sProcessIfdMessages);
	sStartIfdService->addTransition(sStartIfdService, &AbstractState::fireAbort, sStopIfdService);

	sProcessIfdMessages->addTransition(sProcessIfdMessages, &AbstractState::fireContinue, sClearPacePasswordsBeforeStop);
	sProcessIfdMessages->addTransition(sProcessIfdMessages, &AbstractState::fireAbort, sClearPacePasswordsBeforeStop);
	sProcessIfdMessages->addTransition(sProcessIfdMessages, &StateProcessIfdMessages::fireEstablishPaceChannel, sUpdateRetryCounter);
	sProcessIfdMessages->addTransition(sProcessIfdMessages, &StateProcessIfdMessages::fireModifyPin, sPrepareChangePinIfd);
	sProcessIfdMessages->addTransition(sProcessIfdMessages, &StateProcessIfdMessages::fireSecureMessagingStopped, sClearPacePasswords);

	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireContinue, sVerifyRetryCounter);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &AbstractState::fireAbort, sEstablishPaceChannelResponse);
	sUpdateRetryCounter->addTransition(sUpdateRetryCounter, &StateUpdateRetryCounter::fireNoCardConnection, sEstablishPaceChannelResponse);

	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireContinue, sPreparePaceIfd);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &AbstractState::fireAbort, sEstablishPaceChannelResponse);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &StateVerifyRetryCounter::fireNoCardConnection, sEstablishPaceChannelResponse);
	sVerifyRetryCounter->addTransition(sVerifyRetryCounter, &StateVerifyRetryCounter::fireReaderOrCardChanged, sClearPacePasswordsOnNewCard);

	sClearPacePasswordsOnNewCard->addTransition(sClearPacePasswordsOnNewCard, &AbstractState::fireContinue, sPreparePaceIfd);
	sClearPacePasswordsOnNewCard->addTransition(sClearPacePasswordsOnNewCard, &AbstractState::fireAbort, sPreparePaceIfd);

	sPreparePaceIfd->addTransition(sPreparePaceIfd, &AbstractState::fireContinue, sEstablishPaceChannelIfd);
	sPreparePaceIfd->addTransition(sPreparePaceIfd, &AbstractState::fireAbort, sEstablishPaceChannelResponse);
	sPreparePaceIfd->addTransition(sPreparePaceIfd, &StatePreparePaceIfd::fireEnterPacePassword, sEnterPacePasswordIfd);

	sEnterPacePasswordIfd->addTransition(sEnterPacePasswordIfd, &AbstractState::fireContinue, sEstablishPaceChannelIfd);
	sEnterPacePasswordIfd->addTransition(sEnterPacePasswordIfd, &AbstractState::fireAbort, sEstablishPaceChannelResponse);

	sEstablishPaceChannelIfd->addTransition(sEstablishPaceChannelIfd, &AbstractState::fireContinue, sEstablishPaceChannelResponse);
	sEstablishPaceChannelIfd->addTransition(sEstablishPaceChannelIfd, &AbstractState::fireAbort, sEstablishPaceChannelResponse);

	sEstablishPaceChannelResponse->addTransition(sEstablishPaceChannelResponse, &AbstractState::fireContinue, sProcessIfdMessages);
	sEstablishPaceChannelResponse->addTransition(sEstablishPaceChannelResponse, &AbstractState::fireAbort, sProcessIfdMessages);
	sEstablishPaceChannelResponse->addTransition(sEstablishPaceChannelResponse, &StateEstablishPaceChannelResponse::fireWrongPacePassword, sClearPacePasswords);

	sPrepareChangePinIfd->addTransition(sPrepareChangePinIfd, &AbstractState::fireContinue, sChangePinIfd);
	sPrepareChangePinIfd->addTransition(sPrepareChangePinIfd, &AbstractState::fireAbort, sChangePinIfd);
	sPrepareChangePinIfd->addTransition(sPrepareChangePinIfd, &StatePrepareChangePinIfd::fireEnterNewPacePin, sEnterNewPacePinIfd);

	sEnterNewPacePinIfd->addTransition(sEnterNewPacePinIfd, &AbstractState::fireContinue, sChangePinIfd);
	sEnterNewPacePinIfd->addTransition(sEnterNewPacePinIfd, &AbstractState::fireAbort, sChangePinResponse);

	sChangePinIfd->addTransition(sChangePinIfd, &AbstractState::fireContinue, sChangePinResponse);
	sChangePinIfd->addTransition(sChangePinIfd, &AbstractState::fireAbort, sChangePinResponse);

	sChangePinResponse->addTransition(sChangePinResponse, &AbstractState::fireContinue, sProcessIfdMessages);
	sChangePinResponse->addTransition(sChangePinResponse, &AbstractState::fireAbort, sProcessIfdMessages);
	sChangePinResponse->addTransition(sChangePinResponse, &StateChangePinResponse::firePacePasswordModified, sClearPacePasswords);

	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireContinue, sProcessIfdMessages);
	sClearPacePasswords->addTransition(sClearPacePasswords, &AbstractState::fireAbort, sProcessIfdMessages);

	sClearPacePasswordsBeforeStop->addTransition(sClearPacePasswordsBeforeStop, &AbstractState::fireContinue, sStopIfdService);
	sClearPacePasswordsBeforeStop->addTransition(sClearPacePasswordsBeforeStop, &AbstractState::fireAbort, sStopIfdService);

	sStopIfdService->addTransition(sStopIfdService, &AbstractState::fireContinue, sFinal);
	sStopIfdService->addTransition(sStopIfdService, &AbstractState::fireAbort, sFinal);
}
