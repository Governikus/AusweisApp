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

	IdentifyController {}

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
		visible: false
	}

	ProgressView {
		id: identifyProgressView
		leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }
		visible: false
		text: qsTr("Processing")
		subText: qsTr("Please wait a moment...")
	}

	ResultView {
		id: identifyResult
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }
		isError: authModel.resultString
		text: authModel.resultString
		onClicked: {
			authModel.continueWorkflow()
			titleBar.reset()
			identifyView.pop(null)
			navBar.lockedAndHidden = false
		}
		visible: false
	}
}
