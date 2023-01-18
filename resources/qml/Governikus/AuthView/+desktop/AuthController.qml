/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChatModel 1.0
import Governikus.Type.ConnectivityManager 1.0

Controller {
	id: controller
	enum WorkflowStates {
		Initial,
		Reader,
		Update,
		Password,
		Processing
	}

	property bool workflowProgressVisible: false
	property int workflowState: 0

	function processStateChange() {
		switch (AuthModel.currentState) {
		case "Initial":
			break;
		case "StateGetTcToken":
			controller.workflowState = AuthController.WorkflowStates.Initial;
			if (!ConnectivityManager.networkInterfaceActive) {
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
		case "StateWriteHistory":
			showRemoveCardFeedback(AuthModel, true);
			AuthModel.continueWorkflow();
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
			} else if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_SELF_AUTHENTICATION) {
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

	Component.onCompleted: if (AuthModel.currentState === "StateProcessing")
		processStateChange()

	Connections {
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time.
		function onFireCurrentStateChanged(pState) {
			processStateChange();
		}

		target: AuthModel
	}
	Connections {
		function onFireNetworkInterfaceActiveChanged(pActive) {
			processStateChange();
		}

		enabled: AuthModel.currentState === "StateGetTcToken"
		target: ConnectivityManager
	}
}
