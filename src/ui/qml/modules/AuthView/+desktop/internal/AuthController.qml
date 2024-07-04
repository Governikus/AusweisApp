/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.View
import Governikus.Type

Controller {
	id: controller

	enum WorkflowStates {
		Initial,
		Reader,
		Update,
		Password,
		Processing
	}

	readonly property alias networkInterfaceActive: connectivityManager.networkInterfaceActive
	property bool workflowProgressVisible: false
	property int workflowState: 0

	function processStateChange(pState) {
		switch (pState) {
		case "StateGetTcToken":
			controller.workflowState = AuthController.WorkflowStates.Initial;
			if (!networkInterfaceActive) {
				controller.nextView(AuthView.SubViews.Connectivity);
			} else {
				controller.nextView(AuthView.SubViews.Progress);
				AuthModel.continueWorkflow();
			}
			break;
		case "StatePreVerification":
			if (!NumberModel.isCanAllowedMode && SettingsModel.transportPinReminder) {
				SettingsModel.transportPinReminder = false;
				controller.nextView(AuthView.SubViews.TransportPinReminder);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "StateEditAccessRights":
			if (NumberModel.isCanAllowedMode && SettingsModel.skipRightsOnCanAllowed) {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			} else {
				controller.nextView(AuthView.SubViews.AccessRights);
			}
			AuthModel.setInitialPluginType();
			break;
		case "StateSelectReader":
			controller.nextView(AuthView.SubViews.Workflow);
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Reader);
			break;
		case "StateHandleRetryCounter":
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Update);
			break;
		case "StateEnterPacePassword":
			controller.workflowState = AuthController.WorkflowStates.Password;
			if (AuthModel.isBasicReader) {
				controller.nextView(AuthView.SubViews.Password);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "StateUnfortunateCardPosition":
			controller.nextView(AuthView.SubViews.CardPosition);
			break;
		case "StateSendDIDAuthenticateResponseEAC1":
			if (AuthModel.isCancellationByUser()) {
				controller.nextView(AuthView.SubViews.Aborting);
			} else {
				controller.workflowProgressVisible = true;
				controller.nextView(AuthView.SubViews.Progress);
			}
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Processing);
			break;
		case "StateActivateStoreFeedbackDialog":
			if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
				showRemoveCardFeedback(AuthModel, true);
			}
			AuthModel.continueWorkflow();
			break;
		case "StateRedirectBrowser":
			if (!AuthModel.error) {
				controller.nextView(AuthView.SubViews.RedirectToProvider);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "FinalState":
			if (AuthModel.error) {
				if (AuthModel.shouldSkipResultView()) {
					controller.nextView(AuthView.SubViews.ReturnToMain);
					AuthModel.continueWorkflow();
				} else {
					showRemoveCardFeedback(AuthModel, false);
					controller.nextView(AuthView.SubViews.Result);
				}
			} else if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
				controller.nextView(AuthView.SubViews.Data);
			} else {
				controller.nextView(AuthView.SubViews.ReturnToMain);
				AuthModel.continueWorkflow();
			}
			controller.workflowProgressVisible = false;
			break;
		default:
			AuthModel.continueWorkflow();
		}
	}
	function setAuthWorkflowStateAndContinue(pState) {
		controller.workflowState = pState;
		AuthModel.continueWorkflow();
	}

	Component.onCompleted: if (AuthModel.currentState === "StateParseTcTokenUrl")
		processStateChange(AuthModel.currentState)

	Connections {
		function onFireStateEntered(pState) {
			processStateChange(pState);
		}
		function onFireWorkflowFinished() {
			connectivityManager.watching = false;
		}
		function onFireWorkflowStarted() {
			connectivityManager.watching = true;
		}

		target: AuthModel
	}
	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (AuthModel.currentState === "StateGetTcToken")
				processStateChange(AuthModel.currentState);
		}
	}
}
