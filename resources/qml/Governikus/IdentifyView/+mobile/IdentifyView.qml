/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.SelfAuthenticationView 1.0
import Governikus.WhiteListClient 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.ConnectivityManager 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage
{
	id: identifyEditChatView
	navigationAction: NavigationAction {
		state: "back"
		onClicked: appWindow.close()
	}
	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	content: SelfAuthenticationView {
		width: identifyEditChatView.width
		height: identifyEditChatView.height
	}

	IdentifyController {
		id: identifyController
	}

	EditRights {
		id: editRights
		visible: false
	}

	SelfAuthenticationData {
		id: selfAuthenticationData
		visible: false

		onDone: {
			firePop()
			AuthModel.continueWorkflow()
		}
	}

	WhiteListSurveyView {
		id: whiteListSurveyView
		visible: false

		onDone: {
			SettingsModel.setDeviceSurveyPending(pUserAccepted)
			firePop()
			AuthModel.continueWorkflow()
		}
	}

	GeneralWorkflow {
		id: identifyWorkflow
		visible: false

		controller: identifyController
		workflowModel: AuthModel
		//: LABEL ANDROID IOS
		workflowTitle: qsTr("Identify")
	}

	Component {
		id: transportPinReminder

		TransportPinReminderView {}
	}

	EnterPasswordView {
		id: enterPinView

		visible: false

		navigationAction: NavigationAction { state: "cancel"; onClicked: { firePop(); AuthModel.cancelWorkflow() } }
		//: LABEL ANDROID IOS
		title: qsTr("Identify")
		enableTransportPinLink: state === "PIN"

		onPasswordEntered: {
			firePop()
			AuthModel.continueWorkflow()
		}

		onChangePinLength: {
			firePop()
			AuthModel.requestTransportPinChange()
		}
	}

	ProgressView {
		id: identifyAbortedProgressView

		visible: false

		navigationAction: NavigationAction { state: "cancel"; onClicked: AuthModel.cancelWorkflow() }
		//: INFO DESKTOP The user aborted the authentication process, according to TR we need to inform the service provider
		text: qsTr("Aborting process and informing the service provider")
		subText: {
			if (ConnectivityManager.networkInterfaceActive) {
				//: INFO DESKTOP Information message about cancellation process with present network connectivity
				return qsTr("Please wait a moment.")
			}
			//: INFO DESKTOP Information message about cancellation process without working network connectivity
			return qsTr("Network problems detected, trying to reach server within 30 seconds.")
		}
		progressBarVisible: false
		subTextColor: !ConnectivityManager.networkInterfaceActive ? Style.color.warning_text : Style.color.secondary_text
	}

	ProgressView {
		id: identifyProgressView

		readonly property bool inProgress: identifyController.workflowState !== IdentifyController.WorkflowStates.Initial

		navigationAction: NavigationAction {
			state: AuthModel.isBasicReader || identifyController.workflowProgressVisible ? "cancel" : ""
			onClicked: AuthModel.cancelWorkflow()
		}
		//: LABEL ANDROID IOS
		title: qsTr("Identify")
		visible: false
		//: LABEL ANDROID IOS
		text: (AuthModel.error ? qsTr("Cancel authentication process") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				!inProgress ? qsTr("Acquiring provider certificate") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				qsTr("Authentication in progress"))
		subText: {
			if (!visible) {
				return ""
			}
			//: INFO DESKTOP Generic progress status message while no card communication is active.
			if (!inProgress || AuthModel.error) {
				return qsTr("Please wait a moment.")
			}
			if (AuthModel.isBasicReader) {
				//: INFO ANDROID IOS Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
				return qsTr("Please do not move the ID card.")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			if (identifyController.workflowState === IdentifyController.WorkflowStates.Update || identifyController.workflowState === IdentifyController.WorkflowStates.Pin) {
				//: INFO ANDROID IOS The card reader requests the user's attention.
				return qsTr("Please observe the display of your card reader.")
			}
			if (identifyController.workflowState === IdentifyController.WorkflowStates.Can) {
				//: INFO ANDROID IOS The PIN was entered wrongfully two times, the third attempts requires additional CAN verification, hint where the CAN is found.
				return qsTr("A wrong PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.")
			}

			//: INFO ANDROID IOS Generic status message during the authentication process.
			return qsTr("Please wait a moment.")
		}
		subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError
												   || identifyController.workflowState === IdentifyController.WorkflowStates.Can)
					  ? Style.color.warning_text : Style.color.secondary_text
		progressValue: AuthModel.progressValue
		progressText: AuthModel.progressMessage
		progressBarVisible: identifyController.workflowProgressVisible
	}

	ProgressView {
		id: checkConnectivityView
		navigationAction: NavigationAction { state: "cancel"; onClicked: AuthModel.cancelWorkflow() }
		//: LABEL ANDROID IOS
		title: qsTr("Identify")
		visible: false
		//: LABEL ANDROID IOS
		text: qsTr("No network connectivity")
		//: INFO ANDROID IOS No network connection, the user needs to active the network interface or abort the procedure.
		subText: qsTr("Please establish an internet connection.")
		subTextColor: Style.color.warning_text
	}

	ResultView {
		id: cardPositionView
		navigationAction: NavigationAction { state: "cancel"; onClicked: AuthModel.cancelWorkflow() }
		//: LABEL ANDROID IOS
		title: qsTr("Identify")
		resultType: ResultView.Type.IsInfo
		//: LABEL ANDROID IOS
		buttonText: qsTr("Retry")
		//: INFO ANDROID IOS The NFC signal is weak, by repositioning the card the signal might improve.
		text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable")
		onClicked: {
			firePop()
			AuthModel.continueWorkflow()
		}
		visible: false
	}

	ResultView {
		id: identifyResult
		//: LABEL ANDROID IOS
		title: qsTr("Identify")
		resultType: AuthModel.resultString ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		header: AuthModel.errorHeader
		errorCode: AuthModel.statusCodeString
		errorDescription: AuthModel.errorText
		text: AuthModel.resultString
		hintText: AuthModel.statusHintText
		hintButtonText: AuthModel.statusHintActionText
		//: LABEL ANDROID IOS
		contentButton.text: AuthModel.errorIsMasked ? qsTr("Send log") : ""
		contentButton.icon.source: "qrc:///images/material_mail.svg"
		onContentButtonClicked: LogModel.mailLog(qsTr("support@ausweisapp.de"), AuthModel.getEmailHeader(), AuthModel.getEmailBody())
		onHintClicked: AuthModel.invokeStatusHintAction()
		onClicked: {
			AuthModel.continueWorkflow()
			firePopAll()
			if (ApplicationModel.currentWorkflow === "authentication") {
				appWindow.close()
			}
			navBar.lockedAndHidden = false
		}
		visible: false
	}
}
