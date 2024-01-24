/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.EnterPasswordView
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.PasswordInfoView
import Governikus.ProgressView
import Governikus.ResultView
import Governikus.WhiteListClient
import Governikus.View
import Governikus.Workflow
import Governikus.Type.AuthModel
import Governikus.Type.ChatModel
import Governikus.Type.ConnectivityManager
import Governikus.Type.LogModel
import Governikus.Type.NumberModel
import Governikus.Type.PasswordType
import Governikus.Type.ReaderPlugIn
import Governikus.Type.SettingsModel
import Governikus.Type.SurveyModel

Controller {
	id: rootController

	enum WorkflowStates {
		Initial,
		Reader,
		Update,
		Can,
		Pin,
		Puk,
		Processing
	}

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugIn: null
	readonly property bool isInitialState: workflowState === AuthController.WorkflowStates.Initial
	readonly property bool isSmartWorkflow: AuthModel.readerPlugInType === ReaderPlugIn.SMART
	readonly property int passwordType: NumberModel.passwordType
	readonly property bool smartEidUsed: isSmartWorkflow && !isInitialState
	property string title
	property bool workflowProgressVisible: false
	property int workflowState: 0

	signal showChangePinView
	signal workflowFinished

	function processStateChange(pState) {
		switch (pState) {
		case "StateGetTcToken":
			if (connectivityManager.networkInterfaceActive) {
				push(progressView);
				setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Initial);
			} else {
				push(checkConnectivityView);
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
		case "StateEstablishPaceChannel":
			replace(progressView);
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Update);
			break;
		case "StateDidAuthenticateEac1":
			rootController.workflowProgressVisible = true;
			setAuthWorkflowStateAndContinue(AuthController.WorkflowStates.Processing);
			break;
		case "StateSendDIDAuthenticateResponseEAC1":
			if (AuthModel.isCancellationByUser()) {
				replace(authAbortedProgressView);
			}
			AuthModel.continueWorkflow();
			break;
		case "StateActivateStoreFeedbackDialog":
			showRemoveCardFeedback(AuthModel, true);
			AuthModel.continueWorkflow();
			break;
		case "StateShowResult":
			replace(selfAuthenticationData);
			break;
		case "StateSendWhitelistSurvey":
			if (SurveyModel.askForDeviceSurvey()) {
				replace(whiteListSurveyView);
			} else {
				AuthModel.continueWorkflow();
			}
			break;
		case "FinalState":
			if (AuthModel.showChangePinView) {
				AuthModel.continueWorkflow();
				showChangePinView();
			} else if (AuthModel.error && !AuthModel.hasNextWorkflowPending && !AuthModel.shouldSkipResultView()) {
				showRemoveCardFeedback(AuthModel, false);
				replace(authResult);
			} else {
				AuthModel.continueWorkflow();
			}
			rootController.workflowProgressVisible = false;
			break;
		default:
			AuthModel.continueWorkflow();
		}
	}
	function setAuthWorkflowStateAndContinue(pState) {
		rootController.workflowState = pState;
		AuthModel.continueWorkflow();
	}
	function setAuthWorkflowStateAndRequestInput(pState) {
		rootController.workflowState = pState;
		if (AuthModel.isBasicReader) {
			replace(enterPinView, {
					"passwordType": NumberModel.passwordType,
					"inputError": NumberModel.inputError
				});
		} else {
			AuthModel.continueWorkflow();
		}
	}

	Connections {
		function onFireStateEntered(pState) {
			processStateChange(pState);
		}
		function onFireWorkflowFinished() {
			rootController.workflowFinished();
		}

		target: AuthModel
	}
	ConnectivityManager {
		id: connectivityManager

		watching: true

		onNetworkInterfaceActiveChanged: {
			if (AuthModel.currentState === "StateGetTcToken")
				processStateChange(AuthModel.currentState);
		}
	}
	Component {
		id: progressView

		ProgressView {
			progressBarVisible: workflowProgressVisible
			progressText: AuthModel.progressMessage
			progressValue: AuthModel.progressValue
			smartEidUsed: rootController.smartEidUsed
			subText: {
				//: INFO ANDROID IOS Generic status message during the authentication process.
				if (isInitialState || AuthModel.error || isSmartWorkflow) {
					return qsTr("Please wait a moment.");
				}
				if (AuthModel.isBasicReader) {
					//: INFO ANDROID IOS Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
					return qsTr("Please do not move the ID card.");
				}
				if (!!NumberModel.inputError) {
					return NumberModel.inputError;
				}
				if (rootController.workflowState === AuthController.WorkflowStates.Update || rootController.workflowState === AuthController.WorkflowStates.Pin) {
					//: INFO ANDROID IOS The card reader requests the user's attention.
					return qsTr("Please observe the display of your card reader.");
				}
				if (rootController.workflowState === AuthController.WorkflowStates.Can) {
					//: INFO ANDROID IOS The PIN was entered wrongfully two times, the third attempts requires additional CAN verification, hint where the CAN is found.
					return qsTr("A wrong PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
				}

				//: INFO ANDROID IOS Generic status message during the authentication process.
				return qsTr("Please wait a moment.");
			}
			subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError || workflowState === AuthController.WorkflowStates.Can) ? Style.color.text_warning : Style.color.text
			//: LABEL ANDROID IOS
			text: (AuthModel.error ? qsTr("Cancel authentication process") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				isInitialState ? qsTr("Acquiring provider certificate") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				qsTr("Authentication in progress"))
			//: LABEL ANDROID IOS
			title: qsTr("Identify")

			navigationAction: NavigationAction {
				action: AuthModel.isBasicReader || workflowProgressVisible ? NavigationAction.Action.Cancel : NavigationAction.Action.None

				onClicked: AuthModel.cancelWorkflow()
			}
		}
	}
	Component {
		id: editRights

		EditRights {
			title: rootController.title

			onRightsAccepted: {
				ChatModel.transferAccessRights();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: selfAuthenticationData

		SelfAuthenticationData {
			smartEidUsed: rootController.smartEidUsed

			onDone: {
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: whiteListSurveyView

		WhiteListSurveyView {
			smartEidUsed: rootController.smartEidUsed

			onDone: pUserAccepted => {
				SurveyModel.setDeviceSurveyPending(pUserAccepted);
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authWorkflow

		GeneralWorkflow {
			autoInsertCard: rootController.autoInsertCard
			hideSwitch: rootController.hideTechnologySwitch
			initialPlugIn: rootController.initialPlugIn
			smartEidUsed: rootController.smartEidUsed
			workflowModel: AuthModel
			workflowTitle: rootController.title
		}
	}
	Component {
		id: transportPinReminder

		TransportPinReminderView {
			moreInformationText: transportPinReminderInfoData.linkText
			title: rootController.title

			onCancel: AuthModel.cancelWorkflow()
			onPinKnown: {
				pop();
				AuthModel.continueWorkflow();
			}
			onPinUnknown: {
				pop();
				AuthModel.cancelWorkflowToChangePin();
			}
			onShowInfoView: {
				push(transportPinReminderInfoView);
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
	PasswordInfoData {
		id: infoData

		contentType: fromPasswordType(NumberModel.passwordType, NumberModel.isCanAllowedMode)
	}
	Component {
		id: passwordInfoView

		PasswordInfoView {
			infoContent: infoData
			smartEidUsed: rootController.smartEidUsed

			onAbortCurrentWorkflow: AuthModel.cancelWorkflow()
			onClose: pop()
		}
	}
	Component {
		id: enterPinView

		EnterPasswordView {
			//: LABEL ANDROID IOS A11y button to confirm the PIN and start the provider authentication
			accessibleContinueText: passwordType === PasswordType.PIN || passwordType === PasswordType.SMART_PIN || (passwordType === PasswordType.CAN && NumberModel.isCanAllowedMode) ? qsTr("Authenticate with provider") : undefined
			moreInformationText: infoData.linkText
			smartEidUsed: rootController.smartEidUsed
			title: rootController.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					AuthModel.cancelWorkflow();
				}
			}

			onPasswordEntered: {
				AuthModel.continueWorkflow();
			}
			onRequestPasswordInfo: push(passwordInfoView)
		}
	}
	Component {
		id: authAbortedProgressView

		AbortedProgressView {
			networkInterfaceActive: connectivityManager.networkInterfaceActive
			smartEidUsed: rootController.smartEidUsed
			title: rootController.title

			onCancel: AuthModel.cancelWorkflow()
		}
	}
	Component {
		id: checkConnectivityView

		CheckConnectivityView {
			title: rootController.title

			onCancel: AuthModel.cancelWorkflow()
		}
	}
	Component {
		id: cardPositionView

		CardPositionView {
			title: rootController.title

			onCancelClicked: AuthModel.cancelWorkflow()
			onContinueClicked: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authResult

		ResultErrorView {
			errorCode: AuthModel.statusCodeString
			errorDescription: AuthModel.errorText
			header: AuthModel.errorHeader
			hintButtonText: AuthModel.statusHintActionText
			hintText: AuthModel.statusHintText
			icon: AuthModel.statusCodeImage.arg(Style.currentTheme.name)
			//: LABEL ANDROID IOS
			mailButtonText: AuthModel.errorIsMasked ? qsTr("Send log") : ""
			smartEidUsed: rootController.smartEidUsed
			text: AuthModel.resultString
			title: rootController.title

			onContinueClicked: AuthModel.continueWorkflow()
			onHintClicked: {
				AuthModel.continueWorkflow();
				AuthModel.invokeStatusHintAction();
			}
			onMailClicked: LogModel.mailLog("support@ausweisapp.de", AuthModel.getEmailHeader(), AuthModel.getEmailBody())
		}
	}
}
