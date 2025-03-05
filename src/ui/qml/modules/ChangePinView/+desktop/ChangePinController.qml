/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Animations
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.MultiInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.SettingsView
import Governikus.TitleBar
import Governikus.Type
import Governikus.Workflow

ProgressView {
	id: root

	enum WorkflowStates {
		Initial,
		Reader,
		Update,
		Password,
		PinAccepted,
		NewPin,
		Processing
	}

	property ProgressTracker baseProgressTracker: null
	property Component errorViewDelegate: null
	property alias navigationActionType: cancelNavigation.navigationAction
	property Component successViewDelegate: null
	property int workflowState: ChangePinController.WorkflowStates.Initial

	signal workflowFinished(bool pSuccess)

	function continueWorkflowIfComfortReader() {
		if (!ChangePinModel.isBasicReader) {
			ChangePinModel.continueWorkflow();
		}
	}
	function displayInputError() {
		push(inputErrorView, {
			returnCode: ChangePinModel.lastReturnCode,
			inputError: NumberModel.inputError,
			passwordType: NumberModel.passwordType
		});
		continueWorkflowIfComfortReader();
	}
	function displayPinWorkflow() {
		if (!workflowActive) {
			push(pinWorkflow);
		}
	}
	function displaySuccessView(pPasswordType) {
		push(inputSuccessView, {
			passwordType: pPasswordType
		});
		continueWorkflowIfComfortReader();
	}
	function processStateChange(pState) {
		switch (pState) {
		case "StateSelectReader":
			displayPinWorkflow();
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader);
			break;
		case "StatePreparePace":
			pop(root);
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Update);
			break;
		case "StateEnterPacePassword":
			d.setWorkflowProgress(1);
			if (ChangePinModel.lastReturnCode === CardReturnCode.OK_CAN) {
				displaySuccessView(NumberModel.PasswordType.CAN);
				return;
			} else if (ChangePinModel.lastReturnCode === CardReturnCode.OK_PUK) {
				displaySuccessView(NumberModel.PasswordType.PUK);
				return;
			}
			if (NumberModel.inputError !== "") {
				displayInputError();
				return;
			}
			if (NumberModel.initialInputError !== "") {
				push(inputErrorView, {
					returnCode: NumberModel.passwordType === NumberModel.PasswordType.CAN ? CardReturnCode.INVALID_CAN : CardReturnCode.INVALID_PUK,
					inputError: NumberModel.initialInputError,
					passwordType: NumberModel.passwordType,
					titleVisible: false
				});
				NumberModel.setInitialInputErrorShown();
				return;
			}
			setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password);
			break;
		case "StatePrepareChangePin":
			pop(root);
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.PinAccepted);
			d.setWorkflowProgress(3);
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateEnterNewPacePin":
			if (NumberModel.inputError !== "") {
				displayInputError();
				NumberModel.newPin = "";
				NumberModel.newPinConfirmation = "";
				return;
			}
			if (ChangePinModel.requestTransportPin && root.workflowState === ChangePinController.WorkflowStates.PinAccepted) {
				displaySuccessView(NumberModel.PasswordType.TRANSPORT_PIN);
				return;
			}
			setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.NewPin);
			break;
		case "StateCleanUpReaderManager":
			pop(root);
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing);
			break;
		case "FinalState":
			showRemoveCardFeedback(ChangePinModel, false);
			if (ChangePinModel.shouldSkipResultView()) {
				ChangePinModel.continueWorkflow();
				pop(root);
				break;
			}
			d.setWorkflowProgress(progressTracker.steps);
			if (ChangePinModel.error) {
				push(root.errorViewDelegate ? root.errorViewDelegate : pinResult);
			} else {
				push(root.successViewDelegate ? root.successViewDelegate : pinResult);
			}
			break;
		default:
			ChangePinModel.continueWorkflow();
		}
	}
	function rerunCurrentState() {
		processStateChange(ChangePinModel.currentState);
	}
	function setPinWorkflowStateAndContinue(pState) {
		workflowState = pState;
		ChangePinModel.continueWorkflow();
	}
	function setPinWorkflowStateAndRequestInput(pState) {
		workflowState = pState;
		if (ChangePinModel.isBasicReader) {
			push(enterPasswordView);
		} else {
			displayPinWorkflow();
			ChangePinModel.continueWorkflow();
		}
	}

	//: LABEL DESKTOP Processing screen label while the card communication is running after the new PIN has been entered during PIN change process.
	headline: workflowState === ChangePinController.WorkflowStates.NewPin ? qsTr("Setting new ID card PIN") :
	//: LABEL DESKTOP Processing screen label while the card communication is running after the PIN has been entered during PIN check process.
	ChangePinModel.onlyCheckPin ? qsTr("Check ID card PIN") :
	//: LABEL DESKTOP Processing screen label while the card communication is running after the old PIN has been entered during PIN change process.
	qsTr("Change ID card PIN")
	progress: progressTracker
	text: {
		if (ChangePinModel.isBasicReader) {
			//: INFO DESKTOP Processing screen text while the card communication is running after the PIN has been entered during PIN change process.
			return qsTr("Please do not move the ID card.");
		}
		if (workflowState === ChangePinController.WorkflowStates.Update || workflowState === ChangePinController.WorkflowStates.Password || workflowState === ChangePinController.WorkflowStates.NewPin) {
			//: INFO DESKTOP Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
			return qsTr("Please observe the display of your card reader.");
		}
		//: INFO DESKTOP Generic progress message during PIN change process.
		return qsTr("Please wait a moment.");
	}
	//: LABEL DESKTOP
	title: qsTr("Change PIN")
	titleBarSettings: cancelNavigation

	Component.onCompleted: {
		if (ChangePinModel.currentState === "StateMaintainCardConnection") {
			rerunCurrentState();
		}
	}

	ProgressTracker {
		id: progressTracker

		baseProgressTracker: root.baseProgressTracker
		currentStep: d.workflowProgress
		from: 0.2
		steps: ChangePinModel.onlyCheckPin ? 3 : 6
	}
	QtObject {
		id: d

		property int workflowProgress: 0

		function setWorkflowProgress(pProgress) {
			if (d.workflowProgress <= pProgress) {
				d.workflowProgress = pProgress;
			} else {
				console.warn("Tried to update workflowProgress from ", d.workflowProgress, " to ", pProgress);
			}
		}
	}
	MultiInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType, NumberModel.isCanAllowedMode)
	}
	Connections {
		function onFireStateEntered(pState) {
			root.processStateChange(pState);
		}
		function onFireWorkflowFinished(pSuccess) {
			if (pSuccess) {
				SettingsModel.transportPinReminder = false;
			}
			root.workflowFinished(pSuccess);
		}
		function onFireWorkflowStarted() {
			ChangePinModel.setInitialPluginType();
		}

		target: ChangePinModel
	}
	Component {
		id: pinWorkflow

		GeneralWorkflow {
			progress: progressTracker
			title: root.title
			waitingFor: switch (root.workflowState) {
			case ChangePinController.WorkflowStates.Reader:
				return Workflow.WaitingFor.Reader;
			case ChangePinController.WorkflowStates.Update:
				return ChangePinModel.isBasicReader ? Workflow.WaitingFor.None : Workflow.WaitingFor.Password;
			case ChangePinController.WorkflowStates.Password:
			case ChangePinController.WorkflowStates.NewPin:
				return Workflow.WaitingFor.Password;
			default:
				return Workflow.WaitingFor.None;
			}

			titleBarSettings: TitleBarSettings {
				navigationAction: cancelNavigation.navigationAction
				navigationEnabled: ChangePinModel.isBasicReader
				showSettings: true

				onNavigationActionClicked: ChangePinModel.cancelWorkflow()
				onSettingsClicked: root.push(settingsView)
			}

			onSettingsRequested: root.push(settingsView)

			Component {
				id: settingsView

				TabbedReaderView {
					onLeaveView: pop()
				}
			}
		}
	}
	Component {
		id: enterPasswordView

		EnterPasswordView {
			moreInformationText: infoData.linkText
			passwordType: NumberModel.passwordType
			progress: progressTracker
			title: root.title
			titleBarSettings: cancelNavigation

			onPasswordEntered: pPasswordType => {
				pop();
				switch (pPasswordType) {
				case NumberModel.PasswordType.PIN:
				case NumberModel.PasswordType.TRANSPORT_PIN:
					d.setWorkflowProgress(2);
					ChangePinModel.continueWorkflow();
					break;
				case NumberModel.PasswordType.NEW_PIN:
				case NumberModel.PasswordType.NEW_SMART_PIN:
					d.setWorkflowProgress(4);
					root.rerunCurrentState();
					break;
				case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
				case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						d.setWorkflowProgress(5);
						ChangePinModel.continueWorkflow();
					} else {
						root.rerunCurrentState();
					}
					break;
				default:
					ChangePinModel.continueWorkflow();
				}
			}
			onRequestPasswordInfo: push(multiInfoView)
		}
	}
	Component {
		id: inputErrorView

		InputErrorView {
			isTransportPin: ChangePinModel.requestTransportPin
			progress: progressTracker
			title: root.title
			titleBarSettings: cancelNavigation

			onContinueClicked: {
				pop(root);
				if (!ChangePinModel.isBasicReader) {
					root.displayPinWorkflow();
				}
				NumberModel.resetInputError();
				root.rerunCurrentState();
			}
			onPasswordInfoRequested: push(multiInfoView)
		}
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData
			progress: progressTracker

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Back

				onNavigationActionClicked: root.pop()
			}

			onAbortCurrentWorkflow: ChangePinModel.cancelWorkflow()
			onLeaveView: pop()
		}
	}
	Component {
		id: inputSuccessView

		InputSuccessView {
			isTransportPin: ChangePinModel.requestTransportPin
			passwordType: NumberModel.PasswordType.PUK
			progress: progressTracker
			title: root.title
			titleBarSettings: cancelNavigation

			onContinueClicked: {
				pop();
				root.setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Password);
			}
		}
	}
	Component {
		id: cardPositionView

		ResultView {
			animation: AnimationLoader.NFC_RESULT
			animationSymbol: Symbol.Type.ERROR
			progress: progressTracker
			text: ChangePinModel.isRemoteReader ?
			//: INFO DESKTOP The NFC signal is weak or unstable, the user is asked to change the card's position to (hopefully) reduce the distance to the NFC chip.
			qsTr("Weak NFC signal. Please\n change the card position\n remove the mobile phone case (if present)\n connect the smartphone with a charging cable") :
			//: INFO DESKTOP The NFC signal is weak or unstable, while using a stationary card reader.
			qsTr("Weak NFC signal. Please\n make sure the card is positioned correctly on the reader\n do note move the card while it is being accessed")
			title: root.title
			titleBarSettings: cancelNavigation

			onLeaveView: {
				ChangePinModel.continueWorkflow();
				pop();
			}
		}
	}
	Component {
		id: pinResult

		ResultView {
			animation: ChangePinModel.statusCodeAnimation
			//: LABEL DESKTOP
			buttonText: qsTr("Back to start page")
			hintButtonText: ChangePinModel.statusHintActionText
			hintText: ChangePinModel.statusHintText
			hintTitle: ChangePinModel.statusHintTitle
			mailButtonVisible: ChangePinModel.errorIsMasked
			progress: progressTracker
			text: ChangePinModel.resultString
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Cancel
				navigationEnabled: false
			}

			onEmailButtonPressed: ChangePinModel.sendResultMail()
			onHintClicked: ChangePinModel.invokeStatusHintAction()
			onLeaveView: {
				ChangePinModel.continueWorkflow();
				pop(root);
			}
		}
	}
	TitleBarSettings {
		id: cancelNavigation

		navigationAction: NavigationAction.Cancel
		navigationEnabled: ChangePinModel.isBasicReader

		onNavigationActionClicked: {
			root.pop(root);
			ChangePinModel.cancelWorkflow();
		}
	}
}
