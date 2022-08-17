/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AuthController.h"

#include "InternalActivationContext.h"
#include "context/AuthContext.h"
#include "states/CompositeStateTrustedChannel.h"
#include "states/FinalState.h"
#include "states/StateActivateStoreFeedbackDialog.h"
#include "states/StateCheckError.h"
#include "states/StateCheckRefreshAddress.h"
#include "states/StateParseTcTokenUrl.h"
#include "states/StateProcessing.h"
#include "states/StateRedirectBrowser.h"
#include "states/StateSendWhitelistSurvey.h"
#include "states/StateWriteHistory.h"

#include <QDebug>
#include <initializer_list>


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
	auto sCheckError = addState<StateCheckError>();
	auto sActivateStoreFeedbackDialog = addState<StateActivateStoreFeedbackDialog>();
	auto sWriteHistory = addState<StateWriteHistory>();
	auto sSendWhitelistSurvey = addState<StateSendWhitelistSurvey>();
	auto sRedirectBrowser = addState<StateRedirectBrowser>();
	auto sFinal = addState<FinalState>();

	sProcessing->addTransition(sProcessing, &AbstractState::fireContinue, sParseTcTokenUrl);
	sProcessing->addTransition(sProcessing, &AbstractState::fireAbort, sCheckRefreshAddress);

	sParseTcTokenUrl->addTransition(sParseTcTokenUrl, &AbstractState::fireContinue, sTrustedChannel);
	sParseTcTokenUrl->addTransition(sParseTcTokenUrl, &AbstractState::fireAbort, sCheckRefreshAddress);

	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireContinue, sCheckRefreshAddress);
	sTrustedChannel->addTransition(sTrustedChannel, &CompositeStateTrustedChannel::fireAbort, sCheckRefreshAddress);

	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireContinue, sCheckError);
	sCheckRefreshAddress->addTransition(sCheckRefreshAddress, &AbstractState::fireAbort, sRedirectBrowser);

	sCheckError->addTransition(sCheckError, &AbstractState::fireContinue, sActivateStoreFeedbackDialog);
	sCheckError->addTransition(sCheckError, &AbstractState::fireAbort, sRedirectBrowser);

	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireContinue, sWriteHistory);
	sActivateStoreFeedbackDialog->addTransition(sActivateStoreFeedbackDialog, &AbstractState::fireAbort, sWriteHistory);

	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireContinue, sSendWhitelistSurvey);
	sWriteHistory->addTransition(sWriteHistory, &AbstractState::fireAbort, sRedirectBrowser);

	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireContinue, sRedirectBrowser);
	sSendWhitelistSurvey->addTransition(sSendWhitelistSurvey, &AbstractState::fireAbort, sRedirectBrowser);

	sRedirectBrowser->addTransition(sRedirectBrowser, &AbstractState::fireContinue, sFinal);
	sRedirectBrowser->addTransition(sRedirectBrowser, &AbstractState::fireAbort, sFinal);
}


QSharedPointer<WorkflowRequest> AuthController::createWorkflowRequest(const QSharedPointer<ActivationContext>& pActivationContext)
{
	const auto& handler = [](const WorkflowRequest& pRequest, const QSharedPointer<WorkflowRequest>& pActiveWorkflow, const QSharedPointer<WorkflowRequest>& pWaitingWorkflow){
				if (QVector<Action>{Action::AUTH, Action::SELF, Action::PIN}.contains(pActiveWorkflow->getAction()))
				{
					const auto activeContext = pActiveWorkflow->getContext();
					if (activeContext->isWorkflowFinished())
					{
						qDebug() << "Auto-approving the current state";
						if (pWaitingWorkflow.isNull())
						{
							activeContext->setStateApproved(true);
							return WorkflowControl::ENQUEUE;
						}
					}
				}

				auto context = pRequest.getContext().objectCast<AuthContext>();
				if (context && !context->getActivationContext()->sendOperationAlreadyActive())
				{
					qCritical() << "Cannot send \"Operation already active\" to caller:" << context->getActivationContext()->getSendError();
				}

				return WorkflowControl::SKIP;
			};

	return WorkflowRequest::createWorkflowRequestHandler<AuthController, AuthContext>(handler, pActivationContext);
}


QSharedPointer<WorkflowRequest> AuthController::createWorkflowRequest(const QUrl& pUrl)
{
	return createWorkflowRequest(QSharedPointer<InternalActivationContext>::create(pUrl));
}
