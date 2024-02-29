/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.AuthView
import Governikus.EnterPasswordView
import Governikus.ResultView
import Governikus.Style
import Governikus.TitleBar
import Governikus.PasswordInfoView
import Governikus.View
import Governikus.WhiteListClient
import Governikus.Workflow
import Governikus.Type.ConnectivityManager
import Governikus.Type.ChatModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.PersonalizationModel
import Governikus.Type.ReaderPlugIn
import Governikus.Type.SettingsModel
import Governikus.Type.SurveyModel
import Governikus.Type.UiModule

Controller {
	id: controller

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
	property bool workflowProgressVisible: false
	property int workflowState: PersonalizationController.WorkflowStates.Initial

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
			if (PersonalizationModel.showChangePinView) {
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
	function setWorkflowStateAndContinue(pState) {
		controller.workflowState = pState;
		PersonalizationModel.continueWorkflow();
	}
	function setWorkflowStateAndRequestInput(pState) {
		controller.workflowState = pState;
		if (PersonalizationModel.isBasicReader) {
			replace(enterPinView, {
					"passwordType": NumberModel.passwordType,
					"inputError": NumberModel.inputError
				});
		} else {
			replace(smartProgress);
			PersonalizationModel.continueWorkflow();
		}
	}

	Connections {
		function onFireStateEntered(pState) {
			processStateChange(pState);
		}
		function onFireWorkflowFinished() {
			connectivityManager.watching = false;
		}
		function onFireWorkflowStarted() {
			popAll();
			skipSelectReader = false;
			connectivityManager.watching = true;
		}

		target: PersonalizationModel
	}
	ConnectivityManager {
		id: connectivityManager

		onNetworkInterfaceActiveChanged: {
			if (PersonalizationModel.currentState === "StateCheckStatus")
				processStateChange(PersonalizationModel.currentState);
		}
	}
	Component {
		id: transportPinReminder

		TransportPinReminderView {
			moreInformationText: transportPinReminderInfoData.linkText
			smartEidUsed: false
			title: smartEidTitle

			onCancel: PersonalizationModel.cancelWorkflow()
			onPinKnown: {
				pop();
				PersonalizationModel.continueWorkflow();
			}
			onPinUnknown: {
				pop();
				PersonalizationModel.cancelWorkflowToChangePin();
			}
			onShowInfoView: {
				push(transportPinReminderInfoView);
			}
		}
	}
	PasswordInfoData {
		id: transportPinReminderInfoData

		contentType: PasswordInfoData.Type.CHANGE_PIN
	}
	Component {
		id: transportPinReminderInfoView

		PasswordInfoView {
			infoContent: transportPinReminderInfoData
			smartEidUsed: workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid

			onClose: pop()
		}
	}
	Component {
		id: checkConnectivityView

		CheckConnectivityView {
			smartEidUsed: true
			title: smartEidTitle

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}
	Component {
		id: cardPositionView

		CardPositionView {
			smartEidUsed: PersonalizationModel.readerPlugInType === ReaderPlugIn.SMART
			title: smartEidTitle

			onCancelClicked: PersonalizationModel.cancelWorkflow()
			onContinueClicked: {
				pop();
				PersonalizationModel.continueWorkflow();
			}
		}
	}
	Component {
		id: smartProgress

		PersonalizationProgressView {
			progressBarVisible: workflowProgressVisible
			title: smartEidTitle
			workflowState: controller.workflowState

			onAbortWorkflow: {
				controller.workflowState = PersonalizationController.WorkflowStates.Abort;
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
			workflowTitle: smartEidTitle
		}
	}
	PasswordInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType, NumberModel.isCanAllowedMode)
	}
	Component {
		id: passwordInfoView

		PasswordInfoView {
			infoContent: infoData
			smartEidUsed: workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid

			onClose: pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			moreInformationText: infoData.linkText
			smartEidUsed: workflowState !== PersonalizationController.WorkflowStates.Pin
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
				case PasswordType.NEW_SMART_PIN:
					setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.SmartPinNew);
					break;
				case PasswordType.NEW_SMART_PIN_CONFIRMATION:
					if (NumberModel.commitNewPin()) {
						replace(smartProgress);
						if (PersonalizationModel.applet) {
							controller.workflowState = PersonalizationController.WorkflowStates.Personalization;
						} else {
							setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.Personalization);
						}
					} else {
						setWorkflowStateAndRequestInput(PersonalizationController.WorkflowStates.SmartPinNew);
					}
					break;
				default:
					replace(smartProgress);
					PersonalizationModel.continueWorkflow();
				}
			}
			onRequestPasswordInfo: push(passwordInfoView)
		}
	}
	Component {
		id: abortedProgressView

		AbortedProgressView {
			networkInterfaceActive: connectivityManager.networkInterfaceActive
			smartEidUsed: workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid
			title: smartEidTitle

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}
	Component {
		id: resultView

		PersonalizationResultView {
			smartEidUsed: workflowState !== PersonalizationController.WorkflowStates.ProcessingPhysicalEid

			onContinueClicked: {
				if (PersonalizationModel.error) {
					done();
				} else {
					replace(legalInformation);
				}
			}
		}
	}
	Component {
		id: legalInformation

		PersonalizationLegalInformationView {
			onContinueClicked: done()
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
}
