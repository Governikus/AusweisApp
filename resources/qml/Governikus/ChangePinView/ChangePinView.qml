import QtQuick 2.10

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: baseItem

	disableFlicking: true
	headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }

	ChangePinController {
		id: changePinController
	}

	content: ChangePinViewContent {
		height: baseItem.height
		width: baseItem.width
	}

	GeneralWorkflow {
		id: pinWorkflow
		visible: false

		controller: changePinController
		workflowModel: ChangePinModel
		workflowTitle: qsTr("PIN Management") + settingsModel.translationTrigger

		waitingFor: switch (changePinController.workflowState) {
						case ChangePinController.WorkflowStates.Reader:
							return Workflow.WaitingFor.Reader
						case ChangePinController.WorkflowStates.Card:
							return Workflow.WaitingFor.Card
						case ChangePinController.WorkflowStates.Puk:
						case ChangePinController.WorkflowStates.Can:
						case ChangePinController.WorkflowStates.Pin:
						case ChangePinController.WorkflowStates.NewPin:
							return Workflow.WaitingFor.Password
						default:
							return Workflow.WaitingFor.None
		}
	}

	ResultView {
		id: cardPositionView
		headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }
		resultType: ResultView.Type.IsInfo
		buttonText: qsTr("Retry") + settingsModel.translationTrigger
		text: qsTr("Weak NFC signal. Please reposition your card.") + settingsModel.translationTrigger
		onClicked: {
			firePop()
			ChangePinModel.continueWorkflow()
		}
		visible: false
	}

	ResultView {
		id: pinResult
		headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }
		resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		text: ChangePinModel.resultString
		onClicked: {
			ChangePinModel.continueWorkflow()
			firePopAll()
			navBar.lockedAndHidden = false
		}
		visible: false
	}

	EnterPinView {
		id: enterPinView
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: ChangePinModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Change PIN") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			firePop()
			ChangePinModel.continueWorkflow()
		}

		onChangePinLength: NumberModel.requestTransportPin = !NumberModel.requestTransportPin
	}

	ProgressView {
		id: pinProgressView
		leftTitleBarAction: TitleBarAction { state: ChangePinModel.isBasicReader ? "cancel" : "hidden"; onClicked: ChangePinModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }
		visible: false
		text: qsTr("Change PIN") + settingsModel.translationTrigger
		subText: (!visible ? ""
			   : ChangePinModel.isBasicReader ? qsTr("Please wait a moment...")
			   : !!NumberModel.inputError ? NumberModel.inputError
			   : NumberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.")
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Update
				 || changePinController.workflowState === ChangePinController.WorkflowStates.Pin
				 || changePinController.workflowState === ChangePinController.WorkflowStates.NewPin ? qsTr("Please observe the display of your card reader.")
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Can ? qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.")
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Puk ? qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.")
			   : qsTr("Please wait a moment...")) + settingsModel.translationTrigger
		subTextColor: !ChangePinModel.isBasicReader && (NumberModel.inputError
														|| NumberModel.pinDeactivated
														|| changePinController.workflowState === ChangePinController.WorkflowStates.Can
														|| changePinController.workflowState === ChangePinController.WorkflowStates.Puk) ? Constants.red : Constants.secondary_text
	}
}
