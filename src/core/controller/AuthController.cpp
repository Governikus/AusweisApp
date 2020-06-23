/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AuthController.h"

#include "context/AuthContext.h"
#include "states/CompositeStateTrustedChannel.h"
#include "states/FinalState.h"
#include "states/StateActivateStoreFeedbackDialog.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateParseTcTokenUrl.h"
#include "states/StateProcessing.h"
#include "states/StateRedirectBrowser.h"
#include "states/StateSendWhitelistSurvey.h"
#include "states/StateWriteHistory.h"

#include <initializer_list>
#include <QDebug>


using namespace governikus;


AuthController::AuthController(QSharedPointer<AuthContext> pContext)
	: WorkflowController(pContext)
{
	auto sProcessing = addState<StateProcessing>();
	mStateMachine.setInitialState(sProcessing);
	auto sParseTcTokenUrl = addState<StateParseTcTokenUrl>();
	auto sTrustedChannel = new CompositeStateTrustedChannel(pContext);
	mStateMachine.addState(sTrustedChannel);
	auto sCheckRefreshAddress = addState<StateCheckRefreshAddress>();
	auto sActivateStoreFeedbackDialog = addState<StateActivateStoreFeedbackDialog>();
	auto sWriteHistory = addState<StateWriteHistory>();
	auto sRedirectBrowser = addState<StateRedirectBrowser>();
	auto sSendWhitelistSurvey = addState<StateSendWhitelistSurvey>();
	auto sFinal = addState<FinalState>();

	sProcessing->addTransition(sProcessing, &AbstractState::fireContinue, sParseTcTokenUrl);
	sProcessing->addTransition(sProcessing, &AbstractState::fireAbort, sCheckRefreshAddress);

	sParseTcTokenUrl->addTransition(sParseTcTokenUrl, &AbstractState::fireContinue, sTrustedChannel);
	sParseTcTokenUrl->addTransition(sParseTcTokenUrl, &AbstractState::fireAbort, sCheckRefreshAddress);

	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireContinue, sCheckRefreshAddress);
	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireAbort, sCheckRefreshAddress);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sActivateStoreFeedbackDialog);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sRedirectBrowser);

	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireContinue, sWriteHistory);
	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireAbort, sWriteHistory);

	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireContinue, sSendWhitelistSurvey);
	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireAbort, sRedirectBrowser);

	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireContinue, sRedirectBrowser);
	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireAbort, sRedirectBrowser);

	sRedirectBrowser->addTransition(sRedirectBrowser, &AbstractState::fireContinue, sFinal);
	sRedirectBrowser->addTransition(sRedirectBrowser, &AbstractState::fireAbort, sFinal);
}
