import QtQuick 2.10

import Governikus.MainView 1.0
import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.WhiteListClient 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ChangePinModel 1.0


SectionPage
{
	id: identifyEditChatView
	leftTitleBarAction: TitleBarAction {
		state: ApplicationModel.currentWorkflow === "authentication" ? "cancel" : ""
		onClicked: AuthModel.cancelWorkflow()
	}
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }

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

	content: MainView {
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
			settingsModel.setDeviceSurveyPending(pUserAccepted)
			firePop()
			AuthModel.continueWorkflow()
		}
	}

	GeneralWorkflow {
		id: identifyWorkflow
		visible: false

		controller: identifyController
		workflowModel: AuthModel
		workflowTitle: qsTr("Identify") + settingsModel.translationTrigger

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

	EnterPinView {
		id: enterPinView
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: { firePop(); AuthModel.cancelWorkflow() } }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			firePop()
			AuthModel.continueWorkflow()
		}

		onChangePinLength: {
			fireReplace(changeToTransportPinView)
		}
	}

	ProgressView {
		id: identifyProgressView
		leftTitleBarAction: TitleBarAction { state: AuthModel.isBasicReader || identifyController.workflowProgressVisible ? "cancel" : "hidden"; onClicked: AuthModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
		visible: false
		text: (AuthModel.error ? qsTr("Cancel authentication process") :
				identifyController.workflowState === IdentifyController.WorkflowStates.Initial ? qsTr("Acquiring provider certificate") :
				qsTr("Authentication in progress")) + settingsModel.translationTrigger
		subText: {
			settingsModel.translationTrigger;
			if (!visible) {
				return ""
			}
			if (AuthModel.isBasicReader) {
				return qsTr("Please wait a moment...")
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError
			}
			if (NumberModel.pinDeactivated) {
				return qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.")
			}
			if (identifyController.workflowState === IdentifyController.WorkflowStates.Update || identifyController.workflowState === IdentifyController.WorkflowStates.Pin) {
				return qsTr("Please observe the display of your card reader.")
			}
			if (identifyController.workflowState === IdentifyController.WorkflowStates.Can) {
				return qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.")
			}
			return qsTr("Please wait a moment...")
		}
		subTextColor: !AuthModel.isBasicReader && (NumberModel.inputError
												   || NumberModel.pinDeactivated
												   || identifyController.workflowState === IdentifyController.WorkflowStates.Can)
					  ? "red" : Constants.secondary_text
		progressValue: identifyController.workflowProgressValue
		progressText: (progressValue === 0 ? "" :
						progressValue === 1 ? qsTr("Service provider is being verified") :
						progressValue === 2 ? qsTr("Card is being verified") :
						progressValue === 3 ? qsTr("Reading data") :
						progressValue === 4 ? qsTr("Sending data to service provider") :
						progressValue === 5 ? qsTr("Preparing results") :
						"") + settingsModel.translationTrigger
		progressBarVisible: identifyController.workflowProgressVisible
	}

	ProgressView {
		id: checkConnectivityView
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: AuthModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
		visible: false
		text: qsTr("No network connectivity") + settingsModel.translationTrigger
		subText: qsTr("Please enable the network interface or cancel the workflow.") + settingsModel.translationTrigger
		subTextColor: Constants.red
	}

	ResultView {
		id: changeToTransportPinView
		leftTitleBarAction: TitleBarAction { state: "back"; onClicked: fireReplace(enterPinView) }
		headerTitleBarAction: TitleBarAction { text: qsTr("Change transport PIN") + settingsModel.translationTrigger; font.bold: true }
		resultType: ResultView.Type.IsInfo
		buttonText: qsTr("Change PIN") + settingsModel.translationTrigger
		text: qsTr("You have to change your transport PIN into a personal PIN to use the online ID function. You are currently leaving the started process and are forwarded to the PIN management. Please restart the desired process after the PIN has been changed.") + settingsModel.translationTrigger
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
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
		resultType: ResultView.Type.IsInfo
		buttonText: qsTr("Retry") + settingsModel.translationTrigger
		text: qsTr("Weak NFC signal. Please reposition your card.") + settingsModel.translationTrigger
		onClicked: {
			firePop()
			AuthModel.continueWorkflow()
		}
		visible: false
	}

	ResultView {
		id: identifyResult
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
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
