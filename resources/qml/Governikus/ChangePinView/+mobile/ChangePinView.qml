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
import Governikus.View 1.0
import Governikus.Workflow 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: baseItem

	title: changePinController.currentState === "" ?
			//: LABEL ANDROID IOS
			qsTr("Change my (Transport) PIN") :
			NumberModel.requestTransportPin ?
				//: LABEL ANDROID IOS
				qsTr("Change Transport PIN") :
				//: LABEL ANDROID IOS
				qsTr("Change PIN")

	navigationAction: NavigationAction {
		state: "back"
		onClicked: appWindow.close()
	}

	ChangePinController {
		id: changePinController
	}

	Connections {
		target: ChangePinModel
		//: INFO ANDROID IOS The ID card has just been unblocked and the user can now continue with their PIN change.
		onFireOnPinUnlocked: ApplicationModel.showFeedback(qsTr("Your PIN is unblocked. You now have three more attempts to change your PIN."))
	}

	ChangePinViewContent {
		height: baseItem.height
		width: baseItem.width
		onNoPinAvailable: {
			navBar.lockedAndHidden = true
			firePush(pinUnknownView)
		}
		onMoreInformationRequested: firePush(passwordInfoView)
	}

	GeneralWorkflow {
		id: pinWorkflow
		visible: false

		controller: changePinController
		workflowModel: ChangePinModel
		workflowTitle: baseItem.title
	}

	ResultView {
		id: pinUnknownView

		navigationAction: NavigationAction {
			state: "cancel"
			onClicked: {
				firePop()
				navBar.lockedAndHidden = false
			}
		}

		visible: false

		//: LABEL ANDROID IOS
		title: qsTr("No PIN known")
		resultType: ResultView.Type.IsInfo
		buttonText: ""
		text: PinResetInformationModel.pinUnknownText + "\n\n" + PinResetInformationModel.pinUnknownHint
		contentButton.text: PinResetInformationModel.pinResetActionText
		contentButton.icon.source: "qrc:///images/material_open_in_new.svg"

		onContentButtonClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}

	PasswordInfoView {
		id: passwordInfoView

		visible: false

		navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
		changePinInfo: true
	}

	ResultView {
		id: cardPositionView
		navigationAction: NavigationAction { state: "cancel"; onClicked: ChangePinModel.cancelWorkflow() }
		title: baseItem.title
		resultType: ResultView.Type.IsInfo
		//: LABEL ANDROID IOS
		buttonText: qsTr("Retry")
		//: INFO ANDROID IOS The NFC signal is weak or unstable, the user is asked to change the card's position to (hopefully) reduce the distance to the NFC chip.
		text: qsTr("Weak NFC signal. Please\n- change the card position\n- remove the mobile phone case (if present)\n- connect the smartphone with a charging cable")
		onClicked: {
			firePop()
			ChangePinModel.continueWorkflow()
		}
		visible: false
	}

	ResultView {
		id: pinResult
		title: baseItem.title
		resultType: ChangePinModel.error ? ResultView.Type.IsError : ResultView.Type.IsSuccess
		text: ChangePinModel.resultString
		hintText: ChangePinModel.statusHintText
		hintButtonText: ChangePinModel.statusHintActionText
		onHintClicked: ChangePinModel.invokeStatusHintAction()
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
		title: qsTr("Change PIN")
		visible: false

		onPasswordEntered: {
			pinProgressView.wasNewPin = pWasNewPin
			firePop()
			ChangePinModel.continueWorkflow()
		}
	}

	ProgressView {
		id: pinProgressView

		property bool wasNewPin

		navigationAction: NavigationAction {
			state: ChangePinModel.isBasicReader ? "cancel" : "";
			onClicked: ChangePinModel.cancelWorkflow()
		}
		title: baseItem.title
		visible: false
		text: wasNewPin
			//: LABEL ANDROID IOS Processing screen label while the card communication is running after the new PIN has been entered during PIN change process.
			? qsTr("Setting new PIN")
			//: LABEL ANDROID IOS Processing screen label while the card communication is running before the new PIN has been entered during PIN change process.
			: qsTr("Change PIN")
		subText: (!visible ? ""
				 //: INFO ANDROID IOS Loading screen during PIN change process, data communcation is currently ongoing. Message is usually not visible since the password handling with basic reader is handled by EnterPasswordView.
			   : ChangePinModel.isBasicReader ? qsTr("Please do not move the ID card.")
			   : !!NumberModel.inputError ? NumberModel.inputError
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Update
				 || changePinController.workflowState === ChangePinController.WorkflowStates.Pin
				 //: INFO ANDROID IOS Either an comfort card reader or smartphone-as-card-reader is used, the user needs to react to request on that device.
				 || changePinController.workflowState === ChangePinController.WorkflowStates.NewPin ? qsTr("Please observe the display of your card reader.")
				 //: INFO ANDROID IOS The wrong PIN was entered twice, the next attempt requires additional verifcation via CAN.
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Can ? qsTr("A wrong PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.")
				 //: INFO ANDROID IOS The PIN (including the CAN) was entered wrongfully three times, the PUK is required to unlock the ID card.
			   : changePinController.workflowState === ChangePinController.WorkflowStates.Puk ? qsTr("A wrong PIN has been entered three times on your ID card. Your PIN is now blocked. To unblock your PIN you have to enter the PUK.")
				 //: INFO ANDROID IOS Generic progress message during PIN change process.
			   : qsTr("Please do not move the ID card."))
		subTextColor: !ChangePinModel.isBasicReader && (NumberModel.inputError
														|| changePinController.workflowState === ChangePinController.WorkflowStates.Can
														|| changePinController.workflowState === ChangePinController.WorkflowStates.Puk) ? Constants.red : Style.color.secondary_text
	}
}
