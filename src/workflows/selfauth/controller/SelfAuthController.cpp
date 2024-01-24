/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "controller/SelfAuthController.h"

#include "context/SelfAuthContext.h"
#include "states/CompositeStateTrustedChannel.h"
#include "states/FinalState.h"
#include "states/StateActivateStoreFeedbackDialog.h"
#include "states/StateCheckError.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateGetSelfAuthenticationData.h"
#include "states/StateLoadTcTokenUrl.h"
#include "states/StateSendWhitelistSurvey.h"
#include "states/StateShowResult.h"

#include <QDebug>
#include <initializer_list>


using namespace governikus;


SelfAuthController::SelfAuthController(QSharedPointer<SelfAuthContext> pContext)
	: WorkflowController(pContext)
{
	auto sLoadTcTokenUrl = addInitialState<StateLoadTcTokenUrl>();
	auto sTrustedChannel = addState<CompositeStateTrustedChannel>();
	auto sCheckError = addState<StateCheckError>();
	auto sCheckRefreshAddress = addState<StateCheckRefreshAddress>();
	auto sActivateStoreFeedbackDialog = addState<StateActivateStoreFeedbackDialog>();
	auto sSendWhitelistSurvey = addState<StateSendWhitelistSurvey>();
	auto sGetSelfAuthenticationData = addState<StateGetSelfAuthenticationData>();
	auto sShowResult = addState<StateShowResult>();
	auto sFinal = addState<FinalState>();

	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireContinue, sTrustedChannel);
	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireAbort, sCheckRefreshAddress);

	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireContinue, sCheckError);
	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireAbort, sCheckError);

	sCheckError->addTransition(sCheckError, &AbstractState::fireContinue, sCheckRefreshAddress);
	sCheckError->addTransition(sCheckError, &StateCheckError::firePropagateAbort, sFinal);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sActivateStoreFeedbackDialog);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sFinal);

	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireContinue, sGetSelfAuthenticationData);
	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireAbort, sGetSelfAuthenticationData);

	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireContinue, sShowResult);
	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireAbort, sFinal);

	sShowResult->addTransition(sShowResult, &AbstractState::fireContinue, sSendWhitelistSurvey);
	sShowResult->addTransition(sShowResult, &AbstractState::fireAbort, sFinal);

	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireContinue, sFinal);
	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireAbort, sFinal);
}


QSharedPointer<WorkflowRequest> SelfAuthController::createWorkflowRequest(bool pActivateUi)
{
	return WorkflowRequest::create<SelfAuthController, SelfAuthContext>(pActivateUi);
}
