/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.TitleBar
import Governikus.MultiInfoView
import Governikus.View
import Governikus.Workflow
import Governikus.Type

Controller {
	id: root

	property bool autoInsertCard: false
	property Component cardNotActivatedDelegate: null
	property Component errorViewDelegate: null
	property var initialPlugin: null
	property bool isNewPin: false
	readonly property bool isSmartWorkflow: ChangePinModel.readerPluginType === ReaderManagerPluginType.SMART
	property int navigationActionType: NavigationAction.Action.Cancel
	property bool skipProgressView: false
	property bool smartEidUsed: false
	property Component successViewDelegate: null
	property string title

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
	function displaySuccessView(pPasswordType) {
		if (d.lastInputSuccessType === pPasswordType) {
			continueWorkflowIfComfortReader();
			return;
		}

		d.lastInputSuccessType = pPasswordType;
		if (stackView.currentItem instanceof InputSuccessView) {
			replace(inputSuccessView, {
				passwordType: pPasswordType
			});
		} else {
			push(inputSuccessView, {
				passwordType: pPasswordType
			});
		}
		continueWorkflowIfComfortReader();
	}
	function processStateChange(pState) {
		switch (pState) {
		case "StateSelectReader":
			if (!workflowActive) {
				push(pinWorkflow, {
					cardInitiallyAppeared: ChangePinModel.cardInitiallyAppeared
				});
			}
			ChangePinModel.continueWorkflow();
			break;
		case "StatePreparePace":
			if (!root.skipProgressView) {
				replace(pinProgressView);
			}
			ChangePinModel.continueWorkflow();
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
			requestInput();
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateEnterNewPacePin":
			d.setWorkflowProgress(3);
			if (ChangePinModel.requestTransportPin && !root.isNewPin) {
				switch (NumberModel.passwordType) {
				case NumberModel.PasswordType.NEW_PIN_CONFIRMATION:
				case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
					break;
				default:
					displaySuccessView(NumberModel.PasswordType.TRANSPORT_PIN);
					return;
				}
			}
			if (NumberModel.inputError !== "") {
				displayInputError();
				NumberModel.newPin = "";
				NumberModel.newPinConfirmation = "";
				return;
			}
			requestInput();
			break;
		case "FinalState":
			if (ChangePinModel.shouldSkipResultView()) {
				ChangePinModel.continueWorkflow();
				break;
			}
			d.setWorkflowProgress(progressTracker.steps);
			if (ChangePinModel.error && ChangePinModel.statusCode === GlobalStatusCode.Card_Pin_Deactivated) {
				push(root.cardNotActivatedDelegate ? root.cardNotActivatedDelegate : cardNotActivatedView);
				break;
			} else if (ChangePinModel.error) {
				push(root.errorViewDelegate ? root.errorViewDelegate : pinResult);
				break;
			}
			push(root.successViewDelegate ? root.successViewDelegate : pinResult);
			break;
		default:
			ChangePinModel.continueWorkflow();
		}
	}
	function requestInput() {
		if (ChangePinModel.isBasicReader) {
			push(enterPinView, {
				passwordType: NumberModel.passwordType
			});
		} else {
			ChangePinModel.continueWorkflow();
		}
	}
	function rerunCurrentState() {
		processStateChange(ChangePinModel.currentState);
	}

	Component.onCompleted: if (ChangePinModel.currentState === "StateMaintainCardConnection")
		rerunCurrentState()

	QtObject {
		id: d

		property int lastInputSuccessType: -1
		property int workflowProgress: 0

		function setWorkflowProgress(pProgress) {
			if (d.workflowProgress <= pProgress) {
				d.workflowProgress = pProgress;
			} else {
				console.warn("Tried to update workflowProgress from ", d.workflowProgress, " to ", pProgress);
			}
		}
	}
	ProgressTracker {
		id: progressTracker

		baseProgressTracker: root.progress
		currentStep: d.workflowProgress
		steps: ChangePinModel.onlyCheckPin ? 3 : 6
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

		target: ChangePinModel
	}
	Component {
		id: pinWorkflow

		GeneralWorkflow {
			id: workflow

			autoInsertCard: root.autoInsertCard
			initialPlugin: root.initialPlugin
			progress: progressTracker
			smartEidUsed: root.smartEidUsed
			workflowModel: ChangePinModel
			workflowTitle: root.title

			navigationAction: NavigationAction {
				action: root.navigationActionType

				onClicked: ChangePinModel.cancelWorkflow()
			}
		}
	}
	Component {
		id: cardPositionView

		CardPositionView {
			progress: progressTracker
			title: root.title

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
			progress: progressTracker
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: ChangePinModel.isBasicReader ? root.navigationActionType : NavigationAction.Action.None

				onClicked: {
					root.pop();
					ChangePinModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				pop();
				NumberModel.resetInputError();
				root.rerunCurrentState();
			}
			onPasswordInfoRequested: push(multiInfoView)
		}
	}
	Component {
		id: inputSuccessView

		InputSuccessView {
			isTransportPin: ChangePinModel.requestTransportPin
			progress: progressTracker
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: ChangePinModel.isBasicReader ? root.navigationActionType : NavigationAction.Action.None

				onClicked: {
					root.pop();
					ChangePinModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				root.pop();
				root.requestInput();
			}
		}
	}
	Component {
		id: pinResult

		ResultView {
			id: pinResultView

			animation: ChangePinModel.statusCodeAnimation
			//: MOBILE
			buttonText: qsTr("Back to start page")
			firstHintButtonText: ChangePinModel.statusHintActionText
			firstHintText: ChangePinModel.statusHintText
			firstHintTitle: ChangePinModel.statusHintTitle
			hintBoxesTitle: ChangePinModel.statusHintBoxesTitle
			progress: progressTracker
			secondHintButtonLink: PinResetInformationModel.administrativeSearchUrl
			secondHintButtonText: PinResetInformationModel.resetPinAtAuthorityActionText
			secondHintText: Utils.getSecondPRSHintText(ChangePinModel.statusCode)
			secondHintTitle: PinResetInformationModel.resetPinAtAuthorityHintTitle
			smartEidUsed: root.smartEidUsed
			text: ChangePinModel.resultString
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Close

				onClicked: pinResultView.continueClicked()
			}

			onContinueClicked: ChangePinModel.continueWorkflow()
			onFirstHintClicked: {
				ChangePinModel.continueWorkflow();
				ChangePinModel.invokeStatusHintAction();
			}
		}
	}
	MultiInfoData {
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
				return MultiInfoData.Type.TRANSPORT_PIN_NOT_WORKING;
			}
		}
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData
			progress: progressTracker
			smartEidUsed: root.smartEidUsed

			onAbortCurrentWorkflow: ChangePinModel.cancelWorkflow()
			onClose: pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			moreInformationText: infoData.linkText
			progress: progressTracker
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: root.navigationActionType

				onClicked: ChangePinModel.cancelWorkflow()
			}

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
					root.isNewPin = true;
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
		id: pinProgressView

		ProgressView {
			headline: {
				if (root.isSmartWorkflow) {
					return root.isNewPin ?
					//: MOBILE Processing screen label while the card communication is running after the new Smart-eID PIN has been entered during PIN change process.
					qsTr("Setting new Smart-eID PIN") :
					//: MOBILE Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.
					qsTr("Change Smart-eID PIN");
				}
				return root.isNewPin ?
				//: MOBILE Processing screen label while the card communication is running after the new ID card PIN has been entered during PIN change process.
				qsTr("Setting new ID card PIN") :
				//: MOBILE Processing screen label while the card communication is running before the new ID card PIN has been entered during PIN change process.
				qsTr("Change ID card PIN");
			}
			progress: progressTracker
			smartEidUsed: root.smartEidUsed
			text: {
				if (!visible) {
					return "";
				}
				if (root.isSmartWorkflow) {
					//: MOBILE Generic progress message during PIN change process.
					return qsTr("Please wait a moment.");
				}
				if (ChangePinModel.isBasicReader) {
					//: MOBILE Loading screen during PIN change process, data communication is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.
					return qsTr("Please do not move the ID card.");
				}
				//: MOBILE Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
				return qsTr("Please observe the display of your card reader.");
			}
			title: root.title

			navigationAction: NavigationAction {
				action: ChangePinModel.isBasicReader ? root.navigationActionType : NavigationAction.Action.None

				onClicked: ChangePinModel.cancelWorkflow()
			}
		}
	}
	Component {
		id: cardNotActivatedView

		CardNotActivatedView {
			progress: root.progress
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: ChangePinModel.continueWorkflow()
			}
		}
	}
}
