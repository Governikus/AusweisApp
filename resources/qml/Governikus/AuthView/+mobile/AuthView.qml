/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.WhiteListClient 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.SurveyModel 1.0
import Governikus.Type.UiModule 1.0

ProgressView {
	id: root

	readonly property bool isInitialState: authController.workflowState === AuthController.WorkflowStates.Initial
	readonly property bool isSmartWorkflow: AuthModel.readerPlugInType === ReaderPlugIn.SMART

	progressBarVisible: authController.workflowProgressVisible
	progressText: AuthModel.progressMessage
	progressValue: AuthModel.progressValue
	subText: {
		if (!visible) {
			return "";
		}
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
		if (authController.workflowState === AuthController.WorkflowStates.Update || authController.workflowState === AuthController.WorkflowStates.Pin) {
			//: INFO ANDROID IOS The card reader requests the user's attention.
			return qsTr("Please observe the display of your card reader.");
		}
		if (authController.workflowState === AuthController.WorkflowStates.Can) {
			//: INFO ANDROID IOS The PIN was entered wrongfully two times, the third attempts requires additional CAN verification, hint where the CAN is found.
			return qsTr("A wrong PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.");
		}

		//: INFO ANDROID IOS Generic status message during the authentication process.
		return qsTr("Please wait a moment.");
	}
	subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError || authController.workflowState === AuthController.WorkflowStates.Can) ? Style.color.warning_text : Style.color.secondary_text

	//: LABEL ANDROID IOS
	text: (AuthModel.error ? qsTr("Cancel authentication process") :
		//: INFO ANDROID IOS Header of the progress status message during the authentication process.
		isInitialState ? qsTr("Acquiring provider certificate") :
		//: INFO ANDROID IOS Header of the progress status message during the authentication process.
		qsTr("Authentication in progress"))
	//: LABEL ANDROID IOS
	title: qsTr("Identify")
	titleBarColor: isSmartWorkflow && !isInitialState ? Style.color.accent_smart : Style.color.accent

	navigationAction: NavigationAction {
		action: AuthModel.isBasicReader || authController.workflowProgressVisible ? NavigationAction.Action.Cancel : NavigationAction.Action.None

		onClicked: AuthModel.cancelWorkflow()
	}

	AuthController {
		id: authController
	}
	Component {
		id: editRights
		EditRights {
		}
	}
	Component {
		id: selfAuthenticationData
		SelfAuthenticationData {
			titleBarColor: root.titleBarColor

			onDone: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: whiteListSurveyView
		WhiteListSurveyView {
			titleBarColor: root.titleBarColor

			onDone: pUserAccepted => {
				SurveyModel.setDeviceSurveyPending(pUserAccepted);
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authWorkflow
		GeneralWorkflow {
			titleBarColor: root.titleBarColor
			workflowModel: AuthModel
			workflowTitle: root.title
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
			onPinUnknown: {
				pop();
				AuthModel.cancelWorkflowToChangePin();
			}
		}
	}
	Component {
		id: enterPinView
		EnterPasswordView {
			enableTransportPinLink: NumberModel.passwordType === PasswordType.PIN
			title: root.title
			titleBarColor: root.titleBarColor

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: {
					pop();
					AuthModel.cancelWorkflow();
				}
			}

			onChangePinLength: {
				pop();
				AuthModel.requestTransportPinChange();
			}
			onPasswordEntered: {
				pop();
				AuthModel.continueWorkflow();
			}
		}
	}
	Component {
		id: authAbortedProgressView
		AbortedProgressView {
			titleBarColor: root.titleBarColor

			onCancel: AuthModel.cancelWorkflow()
		}
	}
	Component {
		id: checkConnectivityView
		CheckConnectivityView {
			title: root.title

			onCancel: AuthModel.cancelWorkflow()
		}
	}
	Component {
		id: cardPositionView
		CardPositionView {
			title: root.title

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
			property bool skipShowAfterContinue: true

			errorCode: AuthModel.statusCodeString
			errorDescription: AuthModel.errorText
			header: AuthModel.errorHeader
			hintButtonText: AuthModel.statusHintActionText
			hintText: AuthModel.statusHintText
			//: LABEL ANDROID IOS
			mailButtonText: AuthModel.errorIsMasked ? qsTr("Send log") : ""
			resultType: AuthModel.resultString ? ResultView.Type.IsError : ResultView.Type.IsSuccess
			text: AuthModel.resultString
			title: root.title
			titleBarColor: root.titleBarColor

			Component.onDestruction: {
				if (skipShowAfterContinue)
					return;
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_AUTHENTICATION) {
					show(UiModule.DEFAULT);
				} else {
					show(UiModule.SELF_AUTHENTICATION);
				}
			}
			onContinueClicked: {
				AuthModel.continueWorkflow();
				popAll();
				skipShowAfterContinue = false;
			}
			onHintClicked: AuthModel.invokeStatusHintAction()
			onMailClicked: LogModel.mailLog("support@ausweisapp.de", AuthModel.getEmailHeader(), AuthModel.getEmailBody())
		}
	}
}
