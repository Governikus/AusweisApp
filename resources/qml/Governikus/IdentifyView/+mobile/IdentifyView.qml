/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

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
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ChangePinModel 1.0


SectionPage
{
	id: identifyEditChatView
	navigationAction: NavigationAction {
		state: ApplicationModel.currentWorkflow === "authentication" ? "cancel" : ""
		onClicked: AuthModel.cancelWorkflow()
	}
	//: LABEL ANDROID IOS
	title: qsTr("Identify") + SettingsModel.translationTrigger

	// Workaround for "cold start" case, IdentifyController onCompleted fires too early, TabBarView
	// for this View is not ready with loading the initial stack item. firePush of the ProgressView
	// is therefore not caught anywhere. This workaround buffers firePush messages until the base
	// SectionPage is loaded by its parent TabBarView.
	property var pendingSignals: new Array()

	onFirePush: {
		if (!pushed) {
			pendingSignals.push(pSectionPage)
		}
	}

	onPushedChanged: {
		if (pushed) {
			pendingSignals.reverse();
			while (pendingSignals.length > 0) {
				firePush(pendingSignals.pop())
			}
		}
	}

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
		workflowTitle: qsTr("Identify") + SettingsModel.translationTrigger

		waitingFor: switch (identifyController.workflowState) {
						case IdentifyController.WorkflowStates.Reader:
							return Workflow.WaitingFor.Reader
						case IdentifyController.WorkflowStates.Card:
							return Workflow.WaitingFor.Card
						case IdentifyController.WorkflowStates.Can:
						case IdentifyController.WorkflowStates.Pin:
							return Workflow.WaitingFor.Password
						default:
							return Workflow.WaitingFor.None
		}
	}

	EnterPasswordView {
		id: enterPinView
		navigationAction: NavigationAction { state: "cancel"; onClicked: { firePop(); AuthModel.cancelWorkflow() } }
		//: LABEL ANDROID IOS
		title: qsTr("Identify") + SettingsModel.translationTrigger
		visible: false

		onPasswordEntered: {
			firePop()
			AuthModel.continueWorkflow()
		}

		onChangePinLength: {
			fireReplace(changeToTransportPinView)
		}
	}

	ProgressView {
		id: identifyProgressView
		navigationAction: NavigationAction { state: AuthModel.isBasicReader || identifyController.workflowProgressVisible ? "cancel" : "hidden"; onClicked: AuthModel.cancelWorkflow() }
		//: LABEL ANDROID IOS
		title: qsTr("Identify") + SettingsModel.translationTrigger
		visible: false
		//: LABEL ANDROID IOS
		text: (AuthModel.error ? qsTr("Cancel authentication process") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				identifyController.workflowState === IdentifyController.WorkflowStates.Initial ? qsTr("Acquiring provider certificate") :
				//: INFO ANDROID IOS Header of the progress status message during the authentication process.
				qsTr("Authentication in progress")) + SettingsModel.translationTrigger
		subText: {
			SettingsModel.translationTrigger;
			if (!visible) {
				return ""
			}
			if (AuthModel.isBasicReader) {
				//: INFO ANDROID IOS Second line text if a basic card reader is used and background communication with the card/server is running. Is not actually visible since the basic reader password handling is done by EnterPasswordView.
				return qsTr("Please wait a moment...")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			if (NumberModel.pinDeactivated) {
				//: INFO ANDROID IOS The online authentication feature of the id card is disabled and needs to be actived the be authorities.
				return qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.")
			}
			if (identifyController.workflowState === IdentifyController.WorkflowStates.Update || identifyController.workflowState === IdentifyController.WorkflowStates.Pin) {
				//: INFO ANDROID IOS The card reader requests the user's attention.
				return qsTr("Please observe the display of your card reader.")
			}
			if (identifyController.workflowState === IdentifyController.WorkflowStates.Can) {
				//: INFO ANDROID IOS The PIN was entered wrongfully two times, the third attempts requires additional CAN verification, hint where the CAN is found.
				return qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.")
			}
			//: INFO ANDROID IOS Generic status message during the authentication process.
			return qsTr("Please wait a moment...")
		}
		subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError
												   || NumberModel.pinDeactivated
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
		title: qsTr("Identify") + SettingsModel.translationTrigger
		visible: false
		//: LABEL ANDROID IOS
		text: qsTr("No network connectivity") + SettingsModel.translationTrigger
		//: INFO ANDROID IOS No network connection, the user needs to active the network interface or abort the procedure.
		subText: qsTr("Please enable the network interface or cancel the workflow.") + SettingsModel.translationTrigger
		subTextColor: Style.color.warning_text
	}

	ResultView {
		id: changeToTransportPinView
		navigationAction: NavigationAction { state: "back"; onClicked: fireReplace(enterPinView) }
		//: LABEL ANDROID IOS
		title: qsTr("Change transport PIN") + SettingsModel.translationTrigger
		resultType: ResultView.Type.IsInfo
		//: LABEL ANDROID IOS
		buttonText: qsTr("Change PIN") + SettingsModel.translationTrigger
		//: INFO ANDROID IOS The user clicked that the current PIN has 5 digits (transport PIN), it needs to be changed to an ordinary 6 digit PIN. The current process needs to be restarted *manually* by the user.
		text: qsTr("You have to change your transport PIN into a personal PIN to use the online ID function. You are currently leaving the started process and are forwarded to the PIN management. Please restart the desired process after the PIN has been changed.") + SettingsModel.translationTrigger
		onClicked: {
			firePop()
			AuthModel.setSkipRedirect(true)
			ChangePinModel.startWorkflow()
			AuthModel.cancelWorkflowToChangePin()
		}
		visible: false
	}

	ResultView {
		id: cardPositionView
		//: LABEL ANDROID IOS
		title: qsTr("Identify") + SettingsModel.translationTrigger
		resultType: ResultView.Type.IsInfo
		//: LABEL ANDROID IOS
		buttonText: qsTr("Retry") + SettingsModel.translationTrigger
		//: INFO ANDROID IOS The NFC signal is weak, by repositioning the card the signal might improve.
		text: qsTr("Weak NFC signal. Please reposition your card.") + SettingsModel.translationTrigger
		onClicked: {
			firePop()
			AuthModel.continueWorkflow()
		}
		visible: false
	}

	ResultView {
		id: identifyResult
		//: LABEL ANDROID IOS
		title: qsTr("Identify") + SettingsModel.translationTrigger
		resultType: AuthModel.resultString ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		showMailButton: AuthModel.errorIsMasked
		text: AuthModel.resultString
		onClicked: {
			AuthModel.continueWorkflow()
			firePopAll()
			navBar.lockedAndHidden = false
		}
		visible: false
	}
}
