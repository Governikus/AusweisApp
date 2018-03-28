import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import QtQml.StateMachine 1.0 as DSM

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0

SectionPage {
	id: baseItem

	disableFlicking: true
	headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }

	ChangePinController {
		id: changePinController
	}

	content: PinViewContent {
		height: baseItem.height
		width: baseItem.width
	}

	PinWorkflow {
		id: pinWorkflow
		visible: false
	}

	ResultView {
		id: pinResult
		headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }
		isError: changePinModel.error
		text: changePinModel.resultString
		onClicked: {
			numberModel.continueWorkflow()
			firePopAll()
			navBar.lockedAndHidden = false
		}
		visible: false
	}

	EnterPinView {
		id: enterPinView
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: changePinModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Change PIN") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			numberModel.continueWorkflow()
			firePush(pinProgressView, {})
		}
	}

	ProgressView {
		id: pinProgressView
		state: pinWorkflow.state
		leftTitleBarAction: TitleBarAction { state: changePinModel.isBasicReader ? "cancel" : "hidden"; onClicked: changePinModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }
		visible: false
		text: qsTr("Change PIN") + settingsModel.translationTrigger
		subText: (!visible ? "" :
					  changePinModel.isBasicReader ?
							qsTr("Please wait a moment...") :
					  !!numberModel.inputError ?
							numberModel.inputError :
					  numberModel.pinDeactivated ?
							qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.") :
					  (state === "updateretrycounter" || state === "changepin_enterpin" || state === "enternewpin") ?
							qsTr("Please observe the display of your card reader.") :
					  (state === "changepin_entercan") ?
							qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.") :
					  (state === "enterpuk") ?
							qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.") :
					  qsTr("Please wait a moment...")
				 ) + settingsModel.translationTrigger
		subTextColor: !changePinModel.isBasicReader && (numberModel.inputError || numberModel.pinDeactivated || state === "changepin_entercan" || state === "enterpuk") ? "red" : Constants.secondary_text
	}
}
