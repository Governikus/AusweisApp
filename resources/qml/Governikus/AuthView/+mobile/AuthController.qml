/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.ChatModel 1.0
import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SurveyModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.View 1.0

Controller {
	id: controller
	enum WorkflowStates {
		Initial,
		Reader,
		Update,
		Can,
		Pin,
		Puk,
		Processing
	}

	readonly property int passwordType: NumberModel.passwordType
	property bool workflowProgressVisible: false
	property int workflowState: 0

	function processStateChange() {
		switch (AuthModel.currentState) {
		case "Initial":
			popAll();
			break;
		case "StateGetTcToken":
			show(UiModule.IDENTIFY, true);
			popAll();
			if (!ConnectivityManager.networkInterfaceActive) {
				push(checkConnectivityView);
			} else {
				setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Initial);
			}
			break;
		case "StatePreVerification":
			if (!NumberModel.isCanAllowedMode && SettingsModel.transportPinReminder) {
				SettingsModel.transportPinReminder = false;
				push(transportPinReminder);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "StateEditAccessRights":
			if (NumberModel.isCanAllowedMode && SettingsModel.skipRightsOnCanAllowed) {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			} else {
				replace(editRights);
			}
			AuthModel.setInitialPluginType();
			break;
		case "StateSelectReader":
			if (!workflowActive) {
				replace(authWorkflow);
			}
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Reader);
			break;
		case "StatePreparePace":
			popAll();
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Update);
			break;
		case "StateEnterPacePassword":
			if (NumberModel.passwordType === PasswordType.PIN) {
				setAuthWorkflowStateAndRequestInput(AuthController.WorkflowStates.Pin);
			} else if (NumberModel.passwordType === PasswordType.SMART_PIN) {
				setAuthWorkflowStateAndRequestInput(AuthController.WorkflowStates.Pin);
			} else if (NumberModel.passwordType === PasswordType.CAN) {
				setAuthWorkflowStateAndRequestInput(AuthController.WorkflowStates.Can);
			} else if (NumberModel.passwordType === PasswordType.PUK) {
				setAuthWorkflowStateAndRequestInput(AuthController.WorkflowStates.Puk);
			}
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateDidAuthenticateEac1":
			controller.workflowProgressVisible = true;
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Processing);
			break;
		case "StateSendDIDAuthenticateResponseEAC1":
			if (AuthModel.isCancellationByUser()) {
				replace(authAbortedProgressView);
			} else {
				popAll();
			}
			AuthModel.continueWorkflow();
			break;
		case "StateWriteHistory":
			showRemoveCardFeedback(AuthModel, true);
			AuthModel.continueWorkflow();
			break;
		case "StateShowResult":
			push(selfAuthenticationData);
			break;
		case "StateSendWhitelistSurvey":
			if (SurveyModel.askForDeviceSurvey()) {
				push(whiteListSurveyView);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "FinalState":
			if (AuthModel.showChangePinView) {
				AuthModel.continueWorkflow();
				popAll();
				navBar.show(UiModule.PINMANAGEMENT);
			} else if (AuthModel.error && !AuthModel.hasNextWorkflowPending && !AuthModel.shouldSkipResultView()) {
				showRemoveCardFeedback(AuthModel, false);
				push(authResult);
			} else {
				AuthModel.continueWorkflow();
				popAll();
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_AUTHENTICATION) {
					show(UiModule.DEFAULT);
				} else {
					show(UiModule.SELF_AUTHENTICATION);
				}
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
	function setAuthWorkflowStateAndRequestInput(pState) {
		controller.workflowState = pState;
		if (AuthModel.isBasicReader) {
			push(enterPinView);
		} else {
			AuthModel.continueWorkflow();
		}
	}

	Component.onCompleted: if (AuthModel.currentState === "StateProcessing")
		processStateChange()

	Connections {
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
		function onFireCurrentStateChanged() {
			processStateChange();
		}

		target: AuthModel
	}
	Connections {
		function onFireNetworkInterfaceActiveChanged() {
			processStateChange();
		}

		enabled: AuthModel.currentState === "StateGetTcToken"
		target: ConnectivityManager
	}
}
