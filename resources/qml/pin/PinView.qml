import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import QtQml.StateMachine 1.0 as DSM

import "../"
import "../global"

SectionPage {
	id: baseItem

	disableFlicking: true
	headerTitleBarAction: TitleBarAction { text: "Pin"; font.bold: true }

	ChangePinController {}

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
		headerTitleBarAction: TitleBarAction { text: qsTr("Pin"); font.bold: true }
		isError: !changePinModel.changedPinSuccessfully
		text: changePinModel.resultString
		onClicked: {
			changePinModel.continueWorkflow()
			pop(null)
			navBar.lockedAndHidden = false
		}
		visible: false
	}

	EnterPinView {
		id: enterPinView
		visible: false
	}

	ProgressView {
		id: pinProgressView
		leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: changePinModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Pin"); font.bold: true }
		visible: false
		text: qsTr("Processing...")
	}
}
