/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.AuthView 1.0
import Governikus.EnterPasswordView 1.0
import Governikus.ResultView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.PasswordInfoView 1.0
import Governikus.View 1.0
import Governikus.WhiteListClient 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.PersonalizationModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SurveyModel 1.0
import Governikus.Type.UiModule 1.0

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

	readonly property string currentState: PersonalizationModel.currentState
	property bool skipSelectReader: false
	//: LABEL ANDROID IOS
	readonly property string smartEidTitle: qsTr("Smart-eID")
	property bool workflowProgressVisible: false
	property int workflowState: PersonalizationController.WorkflowStates.Initial

	function done() {
		PersonalizationModel.continueWorkflow();
		popAll();
		show(UiModule.SMART);
	}
	function processStateChange() {
		switch (currentState) {
		case "Initial":
			popAll();
			skipSelectReader = false;
			break;
		case "StateCheckStatus":
			show(UiModule.SMART, true);
			popAll();
			if (!ConnectivityManager.networkInterfaceActive) {
				push(checkConnectivityView);
			} else {
				push(smartProgress);
				setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.CheckStatus);
			}
			break;
		case "StatePrepareApplet":
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
			break;
		case "StateGetChallenge":
			setWorkflowStateAndContinue(PersonalizationController.WorkflowStates.Personalization);
			replace(smartProgress);
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
			replace(enterPinView);
		} else {
			replace(smartProgress);
			PersonalizationModel.continueWorkflow();
		}
	}

	Connections {
		// This is necessary because onCurrentStateChanged is not
		// working, when we need to process a state a second time
		function onFireCurrentStateChanged() {
			processStateChange();
		}

		target: PersonalizationModel
	}
	Connections {
		function onFireNetworkInterfaceActiveChanged() {
			processStateChange();
		}

		enabled: currentState === "StateCheckStatus"
		target: ConnectivityManager
	}
	Component {
		id: transportPinReminder
		TransportPinReminderView {
			moreInformationText: transportPinReminderInfoData.linkText
			title: smartEidTitle
			titleBarColor: Style.color.accent

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
				push(transportPinReminderInfoDataView);
			}
		}
	}
	PasswordInfoData {
		id: transportPinReminderInfoData
		contentType: PasswordInfoContent.Type.CHANGE_PIN
	}
	Component {
		id: transportPinReminderInfoView
		PasswordInfoView {
			infoContent: transportPinReminderInfoData

			onClose: pop()
		}
	}
	Component {
		id: checkConnectivityView
		CheckConnectivityView {
			title: smartEidTitle
			titleBarColor: Style.color.accent_smart

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}
	Component {
		id: cardPositionView
		CardPositionView {
			title: smartEidTitle
			titleBarColor: PersonalizationModel.readerPlugInType === ReaderPlugIn.SMART ? Style.color.accent_smart : Style.color.accent

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
			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			titleBarColor: Style.color.accent_smart
			workflowModel: PersonalizationModel
		}
	}
	Component {
		id: smartWorkflow
		GeneralWorkflow {
			titleBarColor: Style.color.accent
			workflowModel: PersonalizationModel
			workflowTitle: smartEidTitle
		}
	}
	Component {
		id: enterPinView
		EnterPasswordView {
			//: LABEL ANDROID IOS
			title: qsTr("Set up Smart-eID")
			titleBarColor: workflowState === PersonalizationController.WorkflowStates.Pin ? Style.color.accent : Style.color.accent_smart

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					PersonalizationModel.cancelWorkflow();
				}
			}

			onPasswordEntered: {
				replace(smartProgress);
				PersonalizationModel.continueWorkflow();
			}
		}
	}
	Component {
		id: abortedProgressView
		AbortedProgressView {
			titleBarColor: workflowState === PersonalizationController.WorkflowStates.ProcessingPhysicalEid ? Style.color.accent : Style.color.accent_smart

			onCancel: PersonalizationModel.cancelWorkflow()
		}
	}
	Component {
		id: resultView
		PersonalizationResultView {
			titleBarColor: workflowState === PersonalizationController.WorkflowStates.ProcessingPhysicalEid ? Style.color.accent : Style.color.accent_smart

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
			titleBarColor: root.titleBarColor

			onDone: pUserAccepted => {
				SurveyModel.setDeviceSurveyPending(pUserAccepted);
				PersonalizationModel.continueWorkflow();
			}
		}
	}
}
