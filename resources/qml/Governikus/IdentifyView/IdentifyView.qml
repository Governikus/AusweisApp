import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls.Styles 1.4

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.ProgressView 1.0
import Governikus.ResultView 1.0

SectionPage
{
	id: identifyEditChatView
	leftTitleBarAction: TitleBarAction {
		state: applicationModel.currentWorkflow === "authentication" ? "cancel" : ""
		onClicked: authModel.cancelWorkflow()
	}
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }

	content: IdentifyViewInfo {
		id: identifyViewInfo
		width: identifyEditChatView.width
		height: identifyEditChatView.height
	}

	IdentifyController {
		id: identifyController
	}

	IdentifyViewContent {
		id: identifyViewContent
		visible: false
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
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			numberModel.continueWorkflow()
			firePush(identifyProgressView, {})
		}
	}

	ProgressView {
		id: identifyProgressView
		state: identifyWorkflow.state
		leftTitleBarAction: TitleBarAction { state: authModel.isBasicReader ? "cancel" : "hidden"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
		visible: false
		text: qsTr("Authentication in progress") + settingsModel.translationTrigger
		subText: (!visible ? "" :
					  authModel.isBasicReader ?
							qsTr("Please wait a moment...") :
					  !!numberModel.inputError ?
							numberModel.inputError :
					  numberModel.pinDeactivated ?
							qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.") :
					  (state === "updateretrycounter" || state === "identify_enterpin") ?
							qsTr("Please observe the display of your card reader.") :
					  (state === "identify_entercan") ?
							qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.") :
					  (state === "enterpuk") ?
							qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.") :
					  qsTr("Please wait a moment...")
				 ) + settingsModel.translationTrigger
		subTextColor: !authModel.isBasicReader && (numberModel.inputError || numberModel.pinDeactivated || state === "identify_entercan" || state === "enterpuk") ? "red" : Constants.secondary_text
		progressValue: 0
		progressText: (progressValue == 0 ? "" :
						progressValue == 1 ? qsTr("Service provider is being verified") :
						progressValue == 2 ? qsTr("Card is being verified") :
						progressValue == 3 ? qsTr("Reading data") :
						progressValue == 4 ? qsTr("Sending data to service provider") :
						progressValue == 5 ? qsTr("Preparing results") :
						"") + settingsModel.translationTrigger
		progressBarVisible: false
	}

	ProgressView {
		id: checkConnectivityView
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
		visible: false
		text: qsTr("No network connectivity") + settingsModel.translationTrigger
		subText: qsTr("Please enable the network interface or cancel the workflow.") + settingsModel.translationTrigger
		subTextColor: Constants.red
	}

	ResultView {
		id: identifyResult
		headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }
		isError: authModel.resultString
		text: authModel.resultString
		onClicked: {
			numberModel.continueWorkflow()
			firePopAll()
			navBar.lockedAndHidden = false
		}
		visible: false
	}
}
