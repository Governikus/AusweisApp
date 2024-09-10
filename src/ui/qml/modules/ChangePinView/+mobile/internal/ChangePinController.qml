/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.AuthView
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.PasswordInfoView
import Governikus.View
import Governikus.Workflow
import Governikus.Type

Controller {
	id: rootController

	enum WorkflowStates {
		Initial,
		Reader,
		Update,
		Puk,
		Can,
		TransportPin,
		Pin,
		NewPin,
		Processing
	}

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugin: null
	property bool isNewPin: false
	property bool smartEidUsed: false
	property string title
	property int workflowState: 0

	signal workflowFinished

	function displayInputError() {
		push(inputErrorView, {
			"returnCode": ChangePinModel.lastReturnCode,
			"inputError": NumberModel.inputError,
			"passwordType": NumberModel.passwordType
		});
	}
	function processStateChange(pState) {
		switch (pState) {
		case "StateSelectReader":
			if (!workflowActive) {
				push(pinWorkflow);
			}
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Reader);
			break;
		case "StatePreparePace":
			replace(pinProgressView);
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Update);
			break;
		case "StateEnterPacePassword":
			if (ChangePinModel.lastReturnCode === CardReturnCode.OK_CAN) {
				push(inputSuccessView, {
					"passwordType": NumberModel.PasswordType.CAN
				});
				return;
			} else if (ChangePinModel.lastReturnCode === CardReturnCode.OK_PUK) {
				push(inputSuccessView, {
					"passwordType": NumberModel.PasswordType.PUK
				});
				return;
			}
			if (NumberModel.inputError !== "") {
				displayInputError();
				return;
			}
			if (NumberModel.initialInputError !== "") {
				push(inputErrorView, {
					"returnCode": NumberModel.passwordType === NumberModel.PasswordType.CAN ? CardReturnCode.INVALID_CAN : CardReturnCode.INVALID_PUK,
					"inputError": NumberModel.initialInputError,
					"passwordType": NumberModel.passwordType,
					"titleVisible": false
				});
				NumberModel.setInitialInputErrorShown();
				return;
			}
			if (NumberModel.passwordType === NumberModel.PasswordType.TRANSPORT_PIN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.TransportPin);
			}
			if (NumberModel.passwordType === NumberModel.PasswordType.PIN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin);
			} else if (NumberModel.passwordType === NumberModel.PasswordType.CAN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Can);
			} else if (NumberModel.passwordType === NumberModel.PasswordType.PUK) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Puk);
			} else if (NumberModel.passwordType === NumberModel.PasswordType.SMART_PIN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin);
			}
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
			setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.NewPin);
			break;
		case "StateCleanUpReaderManager":
			setPinWorkflowStateAndContinue(ChangePinController.WorkflowStates.Processing);
			break;
		case "FinalState":
			showRemoveCardFeedback(ChangePinModel, false);
			if (ChangePinModel.shouldSkipResultView()) {
				ChangePinModel.continueWorkflow();
				setLockedAndHidden(false);
				break;
			}
			push(pinResult);
			break;
		default:
			ChangePinModel.continueWorkflow();
		}
	}
	function rerunCurrentState() {
		processStateChange(ChangePinModel.currentState);
	}
	function setPinWorkflowStateAndContinue(pState) {
		rootController.workflowState = pState;
		ChangePinModel.continueWorkflow();
	}
	function setPinWorkflowStateAndRequestInput(pState) {
		rootController.workflowState = pState;
		if (ChangePinModel.isBasicReader) {
			push(enterPinView, {
				"passwordType": NumberModel.passwordType
			});
		} else {
			ChangePinModel.continueWorkflow();
		}
	}

	Component.onCompleted: if (ChangePinModel.currentState === "StateMaintainCardConnection")
		rerunCurrentState()

	Connections {
		function onFireStateEntered(pState) {
			processStateChange(pState);
		}
		function onFireWorkflowFinished() {
			rootController.workflowFinished();
			popAll();
		}

		target: ChangePinModel
	}
	Component {
		id: pinWorkflow

		GeneralWorkflow {
			autoInsertCard: rootController.autoInsertCard
			hideSwitch: rootController.hideTechnologySwitch
			initialPlugin: rootController.initialPlugin
			smartEidUsed: rootController.smartEidUsed
			workflowModel: ChangePinModel
			workflowTitle: rootController.title
		}
	}
	Component {
		id: cardPositionView

		CardPositionView {
			title: rootController.title

			onCancelClicked: ChangePinModel.cancelWorkflow()
			onContinueClicked: {
				pop();
				ChangePinModel.continueWorkflow();
			}
		}
	}
	Component {
		id: inputErrorView

		InputErrorView {
			isTransportPin: ChangePinModel.requestTransportPin
			smartEidUsed: rootController.smartEidUsed
			title: rootController.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					pop();
					ChangePinModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				pop();
				NumberModel.resetInputError();
				rootController.rerunCurrentState();
			}
			onPasswordInfoRequested: push(passwordInfoView)
		}
	}
	Component {
		id: inputSuccessView

		InputSuccessView {
			isTransportPin: ChangePinModel.requestTransportPin
			smartEidUsed: rootController.smartEidUsed
			title: rootController.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					pop();
					ChangePinModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				pop();
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin);
			}
		}
	}
	Component {
		id: pinResult

		ResultView {
			function endWorkflow() {
				ChangePinModel.continueWorkflow();
				setLockedAndHidden(false);
			}

			//: LABEL ANDROID IOS
			buttonText: qsTr("Back to start page")
			hintButtonText: ChangePinModel.statusHintActionText
			hintText: ChangePinModel.statusHintText
			hintTitle: ChangePinModel.statusHintTitle
			icon: ChangePinModel.error ? ChangePinModel.statusCodeImage.arg(Style.currentTheme.name) : "qrc:///images/workflow_success_changepin_%1.svg".arg(Style.currentTheme.name)
			smartEidUsed: rootController.smartEidUsed
			text: ChangePinModel.resultString
			title: rootController.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel
				enabled: false
			}

			onCancelClicked: continueClicked()
			onContinueClicked: endWorkflow()
			onHintClicked: {
				endWorkflow();
				ChangePinModel.invokeStatusHintAction();
			}
		}
	}
	PasswordInfoData {
		id: infoData

		contentType: {
			if (NumberModel.inputError === "" || !ChangePinModel.requestTransportPin) {
				return fromPasswordType(NumberModel.passwordType);
			}
			switch (ChangePinModel.lastReturnCode) {
			case CardReturnCode.INVALID_CAN:
			case CardReturnCode.INVALID_PUK:
				return fromPasswordType(NumberModel.passwordType);
			default:
				return PasswordInfoData.Type.TRANSPORT_PIN_NOT_WORKING;
			}
		}
	}
	Component {
		id: passwordInfoView

		PasswordInfoView {
			infoContent: infoData
			smartEidUsed: rootController.smartEidUsed

			onAbortCurrentWorkflow: ChangePinModel.cancelWorkflow()
			onClose: pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			moreInformationText: infoData.linkText
			smartEidUsed: rootController.smartEidUsed
			title: rootController.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: ChangePinModel.cancelWorkflow()
			}

			onPasswordEntered: pPasswordType => {
				pop();
				switch (pPasswordType) {
				case NumberModel.PasswordType.NEW_PIN:
				case NumberModel.PasswordType.NEW_SMART_PIN:
					rootController.rerunCurrentState();
					break;
				case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
				case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						rootController.isNewPin = true;
						ChangePinModel.continueWorkflow();
					} else {
						rootController.rerunCurrentState();
					}
					break;
				default:
					ChangePinModel.continueWorkflow();
				}
			}
			onRequestPasswordInfo: push(passwordInfoView)
		}
	}
	Component {
		id: pinProgressView

		ProgressView {
			smartEidUsed: rootController.smartEidUsed
			subText: {
				if (!visible) {
					return "";
				}
				if (isSmartWorkflow) {
					//: INFO ANDROID IOS Generic progress message during PIN change process.
					return qsTr("Please wait a moment.");
				}
				if (ChangePinModel.isBasicReader) {
					//: INFO ANDROID IOS Loading screen during PIN change process, data communication is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.
					return qsTr("Please do not move the ID card.");
				}
				if (rootController.workflowState === ChangePinController.WorkflowStates.Update || rootController.workflowState === ChangePinController.WorkflowStates.Pin || rootController.workflowState === ChangePinController.WorkflowStates.NewPin) {
					//: INFO ANDROID IOS Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
					return qsTr("Please observe the display of your card reader.");
				}
				if (rootController.workflowState === ChangePinController.WorkflowStates.Can) {
					//: INFO ANDROID IOS The wrong ID card PIN was entered twice, the next attempt requires additional verifcation via CAN.
					return qsTr("A wrong ID card PIN has been entered 2 times on your ID card. For a 3rd attempt, please first enter the 6-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
				}
				if (rootController.workflowState === ChangePinController.WorkflowStates.Puk) {
					//: INFO ANDROID IOS The ID card PIN (including the CAN) was entered wrongfully three times, the PUK is required to unlock the ID card.
					return qsTr("You have entered an incorrect, 6-digit ID card PIN 3 times, your ID card PIN is now blocked. To remove the block, the 10-digit PUK must be entered first.");
				}

				//: INFO ANDROID IOS Generic progress message during PIN change process.
				return qsTr("Please wait a moment.");
			}
			subTextColor: Style.color.textNormal.basic
			text: {
				if (isSmartWorkflow) {
					return rootController.isNewPin ?
					//: LABEL ANDROID IOS Processing screen label while the card communication is running after the new Smart-eID PIN has been entered during PIN change process.
					qsTr("Setting new Smart-eID PIN") :
					//: LABEL ANDROID IOS Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.
					qsTr("Change Smart-eID PIN");
				}
				return rootController.isNewPin ?
				//: LABEL ANDROID IOS Processing screen label while the card communication is running after the new ID card PIN has been entered during PIN change process.
				qsTr("Setting new ID card PIN") :
				//: LABEL ANDROID IOS Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.
				qsTr("Change ID card PIN");
			}
			title: rootController.title

			navigationAction: NavigationAction {
				action: ChangePinModel.isBasicReader ? NavigationAction.Action.Cancel : NavigationAction.Action.None

				onClicked: ChangePinModel.cancelWorkflow()
			}
		}
	}
}
