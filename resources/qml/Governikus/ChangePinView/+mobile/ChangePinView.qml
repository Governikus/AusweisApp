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
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: baseItem

	//: LABEL ANDROID IOS
	title: qsTr("PIN Management") + SettingsModel.translationTrigger

	ChangePinController {
		id: changePinController
	}

	Connections {
		target: ChangePinModel
		//: INFO ANDROID IOS The ID card has just been unblocked and the user can now continue with their PIN change.
		onFireOnPinUnlocked: ApplicationModel.showFeedback(qsTr("Your ID card is unblocked. You now have three more tries to change your PIN"))
	}

	ChangePinViewContent {
		height: baseItem.height
		width: baseItem.width
	}

	GeneralWorkflow {
		id: pinWorkflow
		visible: false

		controller: changePinController
		workflowModel: ChangePinModel
		//: LABEL ANDROID IOS
		workflowTitle: qsTr("PIN Management") + SettingsModel.translationTrigger

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
		//: LABEL ANDROID IOS
		title: qsTr("PIN Management") + SettingsModel.translationTrigger
		resultType: ResultView.Type.IsInfo
		//: LABEL ANDROID IOS
		buttonText: qsTr("Retry") + SettingsModel.translationTrigger
		//: INFO ANDROID IOS The NFC signal is weak or unstable, the user is asked to change the card's position to (hopefully) reduce the distance to the NFC chip.
		text: qsTr("Weak NFC signal. Please reposition your card.") + SettingsModel.translationTrigger
		onClicked: {
			firePop()
			ChangePinModel.continueWorkflow()
		}
		visible: false
	}

	ResultView {
		id: pinResult
		//: LABEL ANDROID IOS
		title: qsTr("PIN Management") + SettingsModel.translationTrigger
		resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		text: ChangePinModel.resultString
		onClicked: {
			ChangePinModel.continueWorkflow()
			firePopAll()
			navBar.lockedAndHidden = false
		}
		visible: false
	}

	EnterPasswordView {
		id: enterPinView
		navigationAction: NavigationAction { state: "cancel"; onClicked: ChangePinModel.cancelWorkflow() }
		//: LABEL ANDROID IOS
		title: qsTr("Change PIN") + SettingsModel.translationTrigger
		visible: false

		onPasswordEntered: {
			firePop()
			ChangePinModel.continueWorkflow()
		}

		onChangePinLength: NumberModel.requestTransportPin = !NumberModel.requestTransportPin
	}

	ProgressView {
		id: pinProgressView
		navigationAction: NavigationAction { state: ChangePinModel.isBasicReader ? "cancel" : "hidden"; onClicked: ChangePinModel.cancelWorkflow() }
		//: LABEL ANDROID IOS
		title: qsTr("PIN Management") + SettingsModel.translationTrigger
		visible: false
		//: LABEL ANDROID IOS
		text: qsTr("Change PIN") + SettingsModel.translationTrigger
		subText: (!visible ? ""
				 //: INFO ANDROID IOS Loading screen during PIN change process, data communcation is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.
			   : ChangePinModel.isBasicReader ? qsTr("Please wait a moment...")
			   : !!NumberModel.inputError ? NumberModel.inputError
				 //: INFO ANDROID IOS The card communcation was aborted, the online identification functionality is deactivated and needs to be actived by the authorities.
			   : NumberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.")
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Update
				 || changePinController.workflowState === ChangePinController.WorkflowStates.Pin
				 //: INFO ANDROID IOS Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
				 || changePinController.workflowState === ChangePinController.WorkflowStates.NewPin ? qsTr("Please observe the display of your card reader.")
				 //: INFO ANDROID IOS The wrong PIN was entered twice, the next attempt requires additional verifcation via CAN.
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Can ? qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.")
				 //: INFO ANDROID IOS The PIN (including the CAN) was entered wrongfully three times, the PUK is required to unlock the id card.
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Puk ? qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.")
				 //: INFO ANDROID IOS Generic progress message during PIN change process.
			   : qsTr("Please wait a moment...")) + SettingsModel.translationTrigger
		subTextColor: !ChangePinModel.isBasicReader && (NumberModel.inputError
														|| NumberModel.pinDeactivated
														|| changePinController.workflowState === ChangePinController.WorkflowStates.Can
														|| changePinController.workflowState === ChangePinController.WorkflowStates.Puk) ? Constants.red : Style.color.secondary_text
	}
}
