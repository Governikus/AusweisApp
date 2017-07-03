import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import "../"
import "../global"

SectionPage
{
	id: identifyEditChatView
	leftTitleBarAction: TitleBarMenuAction {
		state: applicationModel.currentWorkflow === "authentication" ? "cancel" : ""
		onClicked: authModel.cancelWorkflow()
	}
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }

	header: IdentifyViewHeader {
		height: identifyEditChatView.height * 0.4
		width: identifyEditChatView.width
	}
	content: IdentifyViewContent {
		width: identifyEditChatView.width
	}

	IdentifyController {
		id: identifyController
	}

	SelfAuthenticationData {
		id: selfAuthenticationData
		visible: false
	}

	IdentifyWorkflow {
		id: identifyWorkflow
		visible: false
	}

	EnterPinView {
		id: enterPinView
		leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") }
		visible: false

		onPinEntered: {
			numberModel.continueWorkflow()
			identifyView.push(identifyProgressView)
		}
	}

	ProgressView {
		id: identifyProgressView
		leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }
		visible: false
		text: qsTr("Processing")
		subText: qsTr("Please wait a moment...")
	}

	ProgressView {
		id: checkConnectivityView
		leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }
		visible: false
		text: qsTr("No network connectivity")
		subText: qsTr("Please enable the network interface or cancel the workflow.")
		subTextColor: Constants.red
	}

	ResultView {
		id: identifyResult
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }
		isError: authModel.resultString
		text: authModel.resultString
		onClicked: {
			numberModel.continueWorkflow()
			titleBar.reset()
			identifyView.pop(null)
			navBar.lockedAndHidden = false
		}
		visible: false
	}
}
