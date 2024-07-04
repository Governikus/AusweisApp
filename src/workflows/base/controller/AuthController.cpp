/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AuthController.h"

#include "context/AuthContext.h"
#include "states/CompositeStateTrustedChannel.h"
#include "states/FinalState.h"
#include "states/StateActivateStoreFeedbackDialog.h"
#include "states/StateCheckError.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateParseTcTokenUrl.h"
#include "states/StateRedirectBrowser.h"
#include "states/StateSendWhitelistSurvey.h"

#include <QDebug>


using namespace governikus;


AuthController::AuthController(QSharedPointer<AuthContext> pContext)
	: WorkflowController(pContext)
{
	auto sParseTcTokenUrl = addInitialState<StateParseTcTokenUrl>();
	auto sTrustedChannel = addState<CompositeStateTrustedChannel>();
	auto sCheckRefreshAddress = addState<StateCheckRefreshAddress>();
	auto sCheckError = addState<StateCheckError>();
	auto sActivateStoreFeedbackDialog = addState<StateActivateStoreFeedbackDialog>();
	auto sSendWhitelistSurvey = addState<StateSendWhitelistSurvey>();
	auto sRedirectBrowser = addState<StateRedirectBrowser>();
	auto sFinal = addState<FinalState>();

	sParseTcTokenUrl->addTransition(sParseTcTokenUrl, &AbstractState::fireContinue, sTrustedChannel);
	sParseTcTokenUrl->addTransition(sParseTcTokenUrl, &AbstractState::fireAbort, sCheckRefreshAddress);

	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireContinue, sCheckRefreshAddress);
	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireAbort, sCheckRefreshAddress);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sCheckError);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sRedirectBrowser);

	sCheckError->addTransition(sCheckError, &AbstractState::fireContinue, sActivateStoreFeedbackDialog);
	sCheckError->addTransition(sCheckError, &AbstractState::fireAbort, sRedirectBrowser);
	sCheckError->addTransition(sCheckError, &StateCheckError::firePropagateAbort, sRedirectBrowser);

	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireContinue, sSendWhitelistSurvey);
	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireAbort, sRedirectBrowser);

	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireContinue, sRedirectBrowser);
	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireAbort, sRedirectBrowser);

	sRedirectBrowser->addTransition(sRedirectBrowser, &AbstractState::fireContinue, sFinal);
	sRedirectBrowser->addTransition(sRedirectBrowser, &AbstractState::fireAbort, sFinal);
}


QSharedPointer<WorkflowRequest> AuthController::createWorkflowRequest(const QUrl& pUrl, const QVariant& pData, const AuthContext::BrowserHandler& pBrowserHandler)
{
	const auto& handler = [](const QSharedPointer<WorkflowRequest>& pActiveWorkflow, const QSharedPointer<WorkflowRequest>& pWaitingWorkflow){
				if (QList<Action>{Action::AUTH, Action::SELF, Action::PIN}.contains(pActiveWorkflow->getAction()))
				{
					const auto activeContext = pActiveWorkflow->getContext();
					if (activeContext->isWorkflowFinished() && pWaitingWorkflow.isNull())
					{
						qDebug() << "Auto-approving the current state";
						if (const auto& context = activeContext.objectCast<AuthContext>(); context)
						{
							context->setMobileSkipRedirect(true);
						}

						activeContext->setStateApproved(true);
						return WorkflowControl::ENQUEUE;
					}
				}

				return WorkflowControl::SKIP;
			};

	return WorkflowRequest::createHandler<AuthController, AuthContext>(handler, pData, true, pUrl, pBrowserHandler);
}
