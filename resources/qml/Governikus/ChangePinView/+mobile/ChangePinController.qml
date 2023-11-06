/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type.ApplicationModel
import Governikus.Type.ChangePinModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.UiModule

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
	property var initialPlugIn: null
	property bool isNewPin: false
	property bool smartEidUsed: false
	property string title
	//: INFO ANDROID IOS
	readonly property string transportPinHint: qsTr("Please note that you may use the five-digit Transport PIN only once to change to a six-digit ID card PIN. If you already set a six-digit ID card PIN, the five-digit Transport PIN is no longer valid.")
	property int workflowState: 0

	signal workflowFinished

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
			if (NumberModel.passwordType === PasswordType.TRANSPORT_PIN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.TransportPin);
			}
			if (NumberModel.passwordType === PasswordType.PIN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin);
			} else if (NumberModel.passwordType === PasswordType.CAN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Can);
			} else if (NumberModel.passwordType === PasswordType.PUK) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Puk);
			} else if (NumberModel.passwordType === PasswordType.SMART_PIN) {
				setPinWorkflowStateAndRequestInput(ChangePinController.WorkflowStates.Pin);
			}
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateEnterNewPacePin":
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
	function setPinWorkflowStateAndContinue(pState) {
		rootController.workflowState = pState;
		ChangePinModel.continueWorkflow();
	}
	function setPinWorkflowStateAndRequestInput(pState) {
		rootController.workflowState = pState;
		if (ChangePinModel.isBasicReader) {
			push(enterPinView, {
					"passwordType": NumberModel.passwordType,
					"inputError": NumberModel.inputError + (NumberModel.inputError !== "" && ChangePinModel.requestTransportPin ? "<br><br>%1".arg(transportPinHint) : "")
				});
		} else {
			ChangePinModel.continueWorkflow();
		}
	}

	Component.onCompleted: if (ChangePinModel.currentState === "StateMaintainCardConnection")
		processStateChange(ChangePinModel.currentState)

	Connections {
		//: INFO ANDROID IOS The ID card has just been unblocked and the user can now continue with their PIN change.
		function onFireOnPinUnlocked() {
			ApplicationModel.showFeedback(qsTr("Your ID card PIN is unblocked. You now have three more attempts to change your PIN."));
		}
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
			initialPlugIn: rootController.initialPlugIn
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
		id: pinResult

		ResultView {
			function endWorkflow() {
				ChangePinModel.continueWorkflow();
				setLockedAndHidden(false);
			}

			hintButtonText: ChangePinModel.statusHintActionText
			hintText: ChangePinModel.statusHintText
			icon: ChangePinModel.error ? ChangePinModel.statusCodeImage.arg(Style.currentTheme.name) : "qrc:///images/workflow_success_changepin_%1.svg".arg(Style.currentTheme.name)
			smartEidUsed: rootController.smartEidUsed
			text: ChangePinModel.resultString
			title: rootController.title

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

		contentType: fromPasswordType(NumberModel.passwordType)
	}
	Component {
		id: passwordInfoView

		PasswordInfoView {
			infoContent: infoData
			smartEidUsed: rootController.smartEidUsed

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: pop()
			}
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
				switch (pPasswordType) {
				case PasswordType.NEW_PIN:
				case PasswordType.NEW_SMART_PIN:
					rootController.processStateChange(ChangePinModel.currentState);
					break;
				case PasswordType.NEW_PIN_CONFIRMATION:
				case PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						pop();
						rootController.isNewPin = true;
						ChangePinModel.continueWorkflow();
					} else {
						rootController.processStateChange(ChangePinModel.currentState);
					}
					break;
				default:
					pop();
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
				if (NumberModel.inputError !== "") {
					return NumberModel.inputError;
				}
				if (rootController.workflowState === ChangePinController.WorkflowStates.Update || rootController.workflowState === ChangePinController.WorkflowStates.Pin || rootController.workflowState === ChangePinController.WorkflowStates.NewPin) {
					//: INFO ANDROID IOS Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
					return qsTr("Please observe the display of your card reader.");
				}
				if (rootController.workflowState === ChangePinController.WorkflowStates.Can) {
					//: INFO ANDROID IOS The wrong ID card PIN was entered twice, the next attempt requires additional verifcation via CAN.
					return qsTr("A wrong ID card PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
				}
				if (rootController.workflowState === ChangePinController.WorkflowStates.Puk) {
					//: INFO ANDROID IOS The ID card PIN (including the CAN) was entered wrongfully three times, the PUK is required to unlock the ID card.
					return qsTr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. To remove the block, the ten-digit PUK must be entered first.");
				}

				//: INFO ANDROID IOS Generic progress message during PIN change process.
				return qsTr("Please wait a moment.");
			}
			subTextColor: !ChangePinModel.isBasicReader && (NumberModel.inputError || rootController.workflowState === ChangePinController.WorkflowStates.Can || rootController.workflowState === ChangePinController.WorkflowStates.Puk) ? Constants.red : Style.color.text
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
