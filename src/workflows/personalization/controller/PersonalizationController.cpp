/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "controller/PersonalizationController.h"

#include "context/PersonalizationContext.h"
#include "states/CompositeStatePace.h"
#include "states/CompositeStatePrepareApplet.h"
#include "states/CompositeStateTrustedChannel.h"
#include "states/FinalState.h"
#include "states/StateActivateStoreFeedbackDialog.h"
#include "states/StateChangeSmartPin.h"
#include "states/StateCheckError.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateCheckStatus.h"
#include "states/StateCleanUpReaderManager.h"
#include "states/StateClearPacePasswords.h"
#include "states/StateDestroyPace.h"
#include "states/StateEnterNewPacePin.h"
#include "states/StateFinalizePersonalization.h"
#include "states/StateGetChallenge.h"
#include "states/StateGetServiceInformation.h"
#include "states/StateGetSessionId.h"
#include "states/StateInitializePersonalization.h"
#include "states/StateInsertCard.h"
#include "states/StateLoadSmartTcTokenUrl.h"
#include "states/StatePreparePersonalization.h"
#include "states/StateSendStartPaosPersonalization.h"
#include "states/StateSendTransmitResponsePersonalization.h"
#include "states/StateSendWhitelistSurvey.h"
#include "states/StateShowResult.h"
#include "states/StateStartPaosPersonalization.h"
#include "states/StateStartPaosPersonalizationResponse.h"
#include "states/StateTransmitPersonalization.h"

#include <initializer_list>


using namespace governikus;


PersonalizationController::PersonalizationController(QSharedPointer<PersonalizationContext> pContext)
	: WorkflowController(pContext)
{
	auto sCheckStatus = addInitialState<StateCheckStatus>();
	auto sPrepareApplet = addState<CompositeStatePrepareApplet>();
	auto sLoadTcTokenUrl = addState<StateLoadSmartTcTokenUrl>();
	auto sTrustedChannel = addState<CompositeStateTrustedChannel>();
	auto sCheckError = addState<StateCheckError>();
	auto sCheckRefreshAddress = addState<StateCheckRefreshAddress>();
	auto sGetSessionId = addState<StateGetSessionId>();
	auto sGetServiceInformation = addState<StateGetServiceInformation>();
	auto sEnterNewPin = addState<StateEnterNewPacePin>();
	auto sGetChallenge = addState<StateGetChallenge>();
	auto sInitializePersonalization = addState<StateInitializePersonalization>();
	auto sPreparePersonalization = addState<StatePreparePersonalization>();
	auto sStartPaos = addState<StateStartPaosPersonalization>();
	auto sSendStartPaos = addState<StateSendStartPaosPersonalization>();
	auto sTransmit = addState<StateTransmitPersonalization>();
	auto sSendTransmitResponse = addState<StateSendTransmitResponsePersonalization>();
	auto sStartPaosResponse = addState<StateStartPaosPersonalizationResponse>();
	auto sFinalizePersonalization = addState<StateFinalizePersonalization>();
	auto sClearPacePasswordsNewPin = addState<StateClearPacePasswords>();
	auto sInsertCard = addState<StateInsertCard>();
	auto sPace = addState<CompositeStatePace>();
	auto sChangeSmartPin = addState<StateChangeSmartPin>();
	auto sDestroyPace = addState<StateDestroyPace>();
	auto sClearPacePasswordsAll = addState<StateClearPacePasswords>();
	auto sCleanUpReaderManager = addState<StateCleanUpReaderManager>();
	auto sActivateStoreFeedbackDialog = addState<StateActivateStoreFeedbackDialog>();
	auto sShowResult = addState<StateShowResult>();
	auto sSendWhitelistSurvey = addState<StateSendWhitelistSurvey>();
	auto sFinal = addState<FinalState>();

	sCheckStatus->addTransition(sCheckStatus, &AbstractState::fireContinue, sPrepareApplet);
	sCheckStatus->addTransition(sCheckStatus, &AbstractState::fireAbort, sFinal);

	sPrepareApplet->addTransition(sPrepareApplet, &CompositeStatePrepareApplet::fireContinue, sLoadTcTokenUrl);
	sPrepareApplet->addTransition(sPrepareApplet, &CompositeStatePrepareApplet::fireAbort, sFinal);

	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireContinue, sTrustedChannel);
	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireAbort, sTrustedChannel);

	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireContinue, sCheckError);
	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireAbort, sCheckError);

	sCheckError->addTransition(sCheckError, &AbstractState::fireContinue, sCheckRefreshAddress);
	sCheckError->addTransition(sCheckError, &AbstractState::fireAbort, sFinal);
	sCheckError->addTransition(sCheckError, &StateCheckError::firePropagateAbort, sFinal);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sGetSessionId);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sFinal);

	sGetSessionId->addTransition(sGetSessionId, &AbstractState::fireContinue, sGetServiceInformation);
	sGetSessionId->addTransition(sGetSessionId, &AbstractState::fireAbort, sFinal);

	sGetServiceInformation->addTransition(sGetServiceInformation, &AbstractState::fireContinue, sEnterNewPin);
	sGetServiceInformation->addTransition(sGetServiceInformation, &AbstractState::fireAbort, sFinal);

	sEnterNewPin->addTransition(sEnterNewPin, &AbstractState::fireContinue, sGetChallenge);
	sEnterNewPin->addTransition(sEnterNewPin, &AbstractState::fireAbort, sFinal);

	sGetChallenge->addTransition(sGetChallenge, &AbstractState::fireContinue, sInitializePersonalization);
	sGetChallenge->addTransition(sGetChallenge, &AbstractState::fireAbort, sClearPacePasswordsNewPin);

	sInitializePersonalization->addTransition(sInitializePersonalization, &AbstractState::fireContinue, sPreparePersonalization);
	sInitializePersonalization->addTransition(sInitializePersonalization, &AbstractState::fireAbort, sClearPacePasswordsNewPin);

	sPreparePersonalization->addTransition(sPreparePersonalization, &AbstractState::fireContinue, sStartPaos);
	sPreparePersonalization->addTransition(sPreparePersonalization, &AbstractState::fireAbort, sFinalizePersonalization);

	sStartPaos->addTransition(sStartPaos, &AbstractState::fireContinue, sSendStartPaos);
	sStartPaos->addTransition(sStartPaos, &AbstractState::fireAbort, sFinalizePersonalization);

	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireContinue, sTransmit);
	sSendStartPaos->addTransition(sSendStartPaos, &AbstractState::fireAbort, sFinalizePersonalization);
	sSendStartPaos->addTransition(sSendStartPaos, &StateSendStartPaosPersonalization::fireReceivedStartPaosResponse, sStartPaosResponse);

	sTransmit->addTransition(sTransmit, &AbstractState::fireContinue, sSendTransmitResponse);
	sTransmit->addTransition(sTransmit, &AbstractState::fireAbort, sSendTransmitResponse);

	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireContinue, sTransmit);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &AbstractState::fireAbort, sFinalizePersonalization);
	sSendTransmitResponse->addTransition(sSendTransmitResponse, &StateSendTransmitResponsePersonalization::fireReceivedStartPaosResponse, sStartPaosResponse);

	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireContinue, sFinalizePersonalization);
	sStartPaosResponse->addTransition(sStartPaosResponse, &AbstractState::fireAbort, sFinalizePersonalization);

	sFinalizePersonalization->addTransition(sFinalizePersonalization, &AbstractState::fireContinue, sInsertCard);
	sFinalizePersonalization->addTransition(sFinalizePersonalization, &AbstractState::fireAbort, sClearPacePasswordsNewPin);
	sFinalizePersonalization->addTransition(sFinalizePersonalization, &StateFinalizePersonalization::firePropagateAbort, sClearPacePasswordsNewPin);

	sClearPacePasswordsNewPin->addTransition(sClearPacePasswordsNewPin, &AbstractState::fireContinue, sFinal);
	sClearPacePasswordsNewPin->addTransition(sClearPacePasswordsNewPin, &AbstractState::fireAbort, sFinal);

	sInsertCard->addTransition(sInsertCard, &AbstractState::fireContinue, sPace);
	sInsertCard->addTransition(sInsertCard, &AbstractState::fireAbort, sClearPacePasswordsAll);
	sInsertCard->addTransition(sInsertCard, &StateInsertCard::fireSkipPinChange, sClearPacePasswordsAll);

	sPace->addTransition(sPace, &CompositeStatePace::fireContinue, sChangeSmartPin);
	sPace->addTransition(sPace, &CompositeStatePace::fireAbort, sClearPacePasswordsAll);
	sPace->addTransition(sPace, &CompositeStatePace::firePropagateAbort, sClearPacePasswordsAll);

	sChangeSmartPin->addTransition(sChangeSmartPin, &AbstractState::fireContinue, sDestroyPace);
	sChangeSmartPin->addTransition(sChangeSmartPin, &AbstractState::fireAbort, sDestroyPace);

	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireContinue, sClearPacePasswordsAll);
	sDestroyPace->addTransition(sDestroyPace, &AbstractState::fireAbort, sClearPacePasswordsAll);

	sClearPacePasswordsAll->addTransition(sClearPacePasswordsAll, &AbstractState::fireContinue, sCleanUpReaderManager);
	sClearPacePasswordsAll->addTransition(sClearPacePasswordsAll, &AbstractState::fireAbort, sCleanUpReaderManager);

	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireContinue, sActivateStoreFeedbackDialog);
	sCleanUpReaderManager->addTransition(sCleanUpReaderManager, &AbstractState::fireAbort, sActivateStoreFeedbackDialog);

	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireContinue, sShowResult);
	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireAbort, sShowResult);

	sShowResult->addTransition(sShowResult, &AbstractState::fireContinue, sSendWhitelistSurvey);
	sShowResult->addTransition(sShowResult, &AbstractState::fireAbort, sFinal);

	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireContinue, sFinal);
	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireAbort, sFinal);
}


QSharedPointer<WorkflowRequest> PersonalizationController::createWorkflowRequest(const QString& pAppletServiceUrl)
{
	return WorkflowRequest::create<PersonalizationController, PersonalizationContext>(pAppletServiceUrl);
}
