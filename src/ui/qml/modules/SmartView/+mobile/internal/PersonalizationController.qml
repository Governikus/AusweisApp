/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.AuthView
import Governikus.EnterPasswordView
import Governikus.ResultView
import Governikus.TitleBar
import Governikus.MultiInfoView
import Governikus.View
import Governikus.WhiteListClient
import Governikus.Workflow
import Governikus.Type

Controller {
	id: root

	enum WorkflowStates {
		Initial,
		CheckStatus,
		InstallApplet,
		TcToken,
		EditRights,
		Reader,
		Pin,
		ProcessingPhysicalEid,
		SmartPinNew,
		Personalization,
		Abort
	}

	property bool skipSelectReader: false
	//: LABEL ANDROID IOS
	readonly property string smartEidTitle: qsTr("Smart-eID")
	property bool smartEidUsed: false
	required property string title
	property bool workflowProgressVisible: false
	property int workflowState: PersonalizationController.WorkflowStates.Initial

	function displayInputError() {
		push(inputErrorView, {
			returnCode: PersonalizationModel.lastReturnCode,
			inputError: NumberModel.inputError,
			passwordType: NumberModel.passwordType
		});
		NumberModel.resetInputError();
	}
	function done() {
		PersonalizationModel.continueWorkflow();
		popAll();
		show(UiModule.SMART_EID);
	}
	function processStateChange(pState) {
		switch (pState) {
		case "StateCheckStatus":
			show(UiModule.SMART_EID, true);
			popAll();
			if (connectivityManager.networkInterfaceActive) {
				push(smartProgress);
				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.CheckStatus);
			} else {
				push(checkConnectivityView);
			}
			break;
		case "StateCheckApplet":
			setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.InstallApplet);
			workflowProgressVisible = true;
			break;
		case "StateLoadSmartTcTokenUrl":
		case "StateGetTcToken":
			workflowProgressVisible = false;
			setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.TcToken);
			break;
		case "StatePreVerification":
			if (!NumberModel.isCanAllowedMode && SettingsModel.transportPinReminder) {
				SettingsModel.transportPinReminder = false;
				push(transportPinReminder);
			} else {
				PersonalizationModel.continueWorkflow();
			}
			break;
		case "StateEditAccessRights":
			workflowState = PersonalizationController.WorkflowStates.EditRights;
			replace(editRights);
			PersonalizationModel.setInitialPluginType();
			break;
		case "StateSelectReader":
			if (skipSelectReader) {
				PersonalizationModel.continueWorkflow();
				break;
			}
			setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.Reader);
			if (!workflowActive) {
				replace(smartWorkflow);
			}
			break;
		case "StateEnterPacePassword":
			if (PersonalizationModel.lastReturnCode === CardReturnCode.OK_CAN) {
				push(inputSuccessView, {
					passwordType: NumberModel.PasswordType.CAN
				});
				return;
			} else if (PersonalizationModel.lastReturnCode === CardReturnCode.OK_PUK) {
				push(inputSuccessView, {
					passwordType: NumberModel.PasswordType.PUK
				});
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
			setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.Pin);
			break;
		case "StateEstablishPaceChannel":
			if (skipSelectReader) {
				PersonalizationModel.continueWorkflow();
				break;
			}
			setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.ProcessingPhysicalEid);
			replace(smartProgress);
			break;
		case "StateUnfortunateCardPosition":
			push(cardPositionView);
			break;
		case "StateDidAuthenticateEac1":
			if (stackView.currentItem instanceof InputSuccessView) {
				pop();
			}
			setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.ProcessingPhysicalEid);
			workflowProgressVisible = true;
			break;
		case "StateSendDIDAuthenticateResponseEAC1":
			if (PersonalizationModel.isCancellationByUser()) {
				replace(abortedProgressView);
			}
			PersonalizationModel.continueWorkflow();
			break;
		case "StateGetSessionId":
			skipSelectReader = true;
			showRemoveCardFeedback(PersonalizationModel, false);
			PersonalizationModel.continueWorkflow();
			break;
		case "StateEnterNewPacePin":
			if (NumberModel.inputError !== "") {
				displayInputError();
				NumberModel.newPin = "";
				NumberModel.newPinConfirmation = "";
				return;
			}
			setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.SmartPinNew);
			if (PersonalizationModel.applet) {
				PersonalizationModel.continueWorkflow();
			}
			break;
		case "StateShowResult":
			if (!PersonalizationModel.error)
				replace(resultView);
			else
				PersonalizationModel.continueWorkflow();
			break;
		case "StateSendWhitelistSurvey":
			if (SurveyModel.askForDeviceSurvey()) {
				replace(whiteListSurveyView);
			} else {
				PersonalizationModel.continueWorkflow();
			}
			break;
		case "FinalState":
			if (PersonalizationModel.changeTransportPin) {
				done();
				show(UiModule.PINMANAGEMENT);
			} else if (PersonalizationModel.error && !PersonalizationModel.hasNextWorkflowPending && !PersonalizationModel.shouldSkipResultView()) {
				replace(resultView);
			} else {
				done();
			}
			workflowProgressVisible = false;
			break;
		default:
			PersonalizationModel.continueWorkflow();
		}
	}
	function rerunCurrentState() {
		processStateChange(PersonalizationModel.currentState);
	}
	function setWorkflowStateAndContinue(pState) {
		root.workflowState = pState;
		PersonalizationModel.continueWorkflow();
	}
	function setWorkflowStateAndRequestInput(pState) {
		root.workflowState = pState;
		if (PersonalizationModel.isBasicReader) {
			replace(enterPinView, {
				passwordType: NumberModel.passwordType
			});
		} else {
			replace(smartProgress);
			PersonalizationModel.continueWorkflow();
		}
	}

	Connections {
		function onFireStateEntered(pState) {
			root.processStateChange(pState);
		}
		function onFireWorkflowFinished(pSuccess) {
			connectivityManager.watching = false;
		}
		function onFireWorkflowStarted() {
			root.popAll();
			root.skipSelectReader = false;
			connectivityManager.watching = true;
		}

		target: PersonalizationModel
	}
	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (PersonalizationModel.currentState === "StateCheckStatus")
				root.rerunCurrentState();
		}
	}
	Component {
		id: transportPinReminder

		TransportPinReminderView {
			title: root.title

			onCancel: AuthModel.cancelWorkflow()
			onPinKnown: {
				pop();
				AuthModel.continueWorkflow();
			}
			onTransportPinKnown: {
				pop();
				AuthModel.cancelWorkflowToChangeTransportPin();
			}
		}
	}
	Component {
		id: checkConnectivityView

		CheckConnectivityView {
			smartEidUsed: true
			title: root.smartEidTitle

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}
	Component {
		id: cardPositionView

		CardPositionView {
			smartEidUsed: PersonalizationModel.readerPluginType === ReaderManagerPluginType.SMART
			title: root.smartEidTitle

			onCancelClicked: PersonalizationModel.cancelWorkflow()
			onContinueClicked: {
				root.pop();
				PersonalizationModel.continueWorkflow();
			}
		}
	}
	Component {
		id: smartProgress

		PersonalizationProgressView {
			progressBarVisible: root.workflowProgressVisible
			title: root.smartEidTitle
			workflowState: root.workflowState

			onAbortWorkflow: {
				root.workflowState = PersonalizationController.WorkflowStates.Abort;
				PersonalizationModel.cancelWorkflow();
			}
		}
	}
	Component {
		id: editRights

		EditRights {
			//: INFO ANDROID IOS The user is informed that the ID card needs to be read to create a Smart-eID.
			actionText: qsTr("The Smart-eID issuing authority needs to read your ID card's data in order to store it on this device:")
			//: LABEL IOS_PHONE ANDROID_PHONE
			dataText: qsTr("By entering your ID card PIN, access to the following data of your ID card will be allowed to the mentioned provider:")
			smartEidUsed: true
			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			workflowModel: PersonalizationModel

			onRightsAccepted: {
				ChatModel.transferAccessRights();
				PersonalizationModel.continueWorkflow();
			}
		}
	}
	Component {
		id: smartWorkflow

		GeneralWorkflow {
			smartEidUsed: false
			workflowModel: PersonalizationModel
			workflowTitle: root.smartEidTitle
		}
	}
	MultiInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType, NumberModel.isCanAllowedMode)
	}
	Component {
		id: multiInfoView

		MultiInfoView {
			infoContent: infoData
			smartEidUsed: root.workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid

			onClose: root.pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			moreInformationText: infoData.linkText
			smartEidUsed: root.workflowState !== PersonalizationController.WorkflowStates.Pin
			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					PersonalizationModel.cancelWorkflow();
				}
			}

			onPasswordEntered: pPasswordType => {
				switch (pPasswordType) {
				case NumberModel.PasswordType.NEW_SMART_PIN:
					root.setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.SmartPinNew);
					break;
				case NumberModel.PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						root.replace(smartProgress);
						if (PersonalizationModel.applet) {
							root.workflowState = PersonalizationController.WorkflowStates.Personalization;
						} else {
							root.setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.Personalization);
						}
					} else {
						root.setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.SmartPinNew);
					}
					break;
				default:
					root.replace(smartProgress);
					PersonalizationModel.continueWorkflow();
				}
			}
			onRequestPasswordInfo: root.push(multiInfoView)
		}
	}
	Component {
		id: abortedProgressView

		AbortedProgressView {
			networkInterfaceActive: connectivityManager.networkInterfaceActive
			smartEidUsed: root.workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid
			title: root.smartEidTitle

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}
	Component {
		id: resultView

		PersonalizationResultView {
			smartEidUsed: root.workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid

			onContinueClicked: {
				if (PersonalizationModel.error) {
					root.done();
				} else {
					root.replace(legalInformation);
				}
			}
		}
	}
	Component {
		id: legalInformation

		PersonalizationLegalInformationView {
			onContinueClicked: root.done()
		}
	}
	Component {
		id: whiteListSurveyView

		WhiteListSurveyView {
			smartEidUsed: root.smartEidUsed

			onDone: pUserAccepted => {
				SurveyModel.setDeviceSurveyPending(pUserAccepted);
				PersonalizationModel.continueWorkflow();
			}
		}
	}
	Component {
		id: inputErrorView

		InputErrorView {
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					root.pop();
					PersonalizationModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				root.pop();
				root.rerunCurrentState();
			}
			onPasswordInfoRequested: push(multiInfoView)
		}
	}
	Component {
		id: inputSuccessView

		InputSuccessView {
			smartEidUsed: root.smartEidUsed
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					root.pop();
					PersonalizationModel.cancelWorkflow();
				}
			}

			onContinueClicked: {
				root.pop();
				root.setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.Pin);
			}
		}
	}
}
