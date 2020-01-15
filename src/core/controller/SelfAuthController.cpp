/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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
#include "states/StateShowSelfInfo.h"
#include "states/StateWriteHistory.h"

#include <initializer_list>
#include <QDebug>


using namespace governikus;


SelfAuthController::SelfAuthController(QSharedPointer<SelfAuthContext> pContext)
	: WorkflowController(pContext)
{
	auto sLoadTcTokenUrl = addState<StateLoadTcTokenUrl>();
	mStateMachine.setInitialState(sLoadTcTokenUrl);
	auto sTrustedChannel = new CompositeStateTrustedChannel(pContext);
	mStateMachine.addState(sTrustedChannel);
	auto sCheckErrorEpilogue = addState<StateCheckError>();
	auto sCheckRefreshAddress = addState<StateCheckRefreshAddress>();
	auto sActivateStoreFeedbackDialog = addState<StateActivateStoreFeedbackDialog>();
	auto sWriteHistory = addState<StateWriteHistory>();
	auto sSendWhitelistSurvey = addState<StateSendWhitelistSurvey>();
	auto sGetSelfAuthenticationData = addState<StateGetSelfAuthenticationData>();
	auto sShowSelfInfo = addState<StateShowSelfInfo>();
	auto sFinal = addState<FinalState>();

	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireContinue, sTrustedChannel);
	sLoadTcTokenUrl->addTransition(sLoadTcTokenUrl, &AbstractState::fireAbort, sCheckRefreshAddress);

	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireContinue, sCheckErrorEpilogue);
	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireAbort, sCheckErrorEpilogue);

	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireContinue, sCheckRefreshAddress);
	sCheckErrorEpilogue->addTransition(sCheckErrorEpilogue, &AbstractState::fireAbort, sFinal);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sActivateStoreFeedbackDialog);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sFinal);

	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireContinue, sWriteHistory);
	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireAbort, sWriteHistory);

	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireContinue, sGetSelfAuthenticationData);
	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireAbort, sGetSelfAuthenticationData);

	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireContinue, sShowSelfInfo);
	sGetSelfAuthenticationData->addTransition(sGetSelfAuthenticationData, &AbstractState::fireAbort, sFinal);

	sShowSelfInfo->addTransition(sShowSelfInfo, &AbstractState::fireContinue, sSendWhitelistSurvey);
	sShowSelfInfo->addTransition(sShowSelfInfo, &AbstractState::fireAbort, sFinal);

	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireContinue, sFinal);
	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireAbort, sFinal);
}
