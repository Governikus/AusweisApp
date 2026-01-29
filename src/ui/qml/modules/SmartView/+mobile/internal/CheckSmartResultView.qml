/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Animations
import Governikus.CheckResultView
import Governikus.Type

CheckResultSuggestionView {
	id: root

	property int result

	signal checkDevice
	signal runSmartSetup

	Accessible.name: suggestionData.title
	smartEidUsed: true
	suggestionData: {
		if (SmartModel.errorString !== "") {
			return updateCheckError;
		}
		switch (result) {
		case SmartModel.State.UPDATING_STATUS:
			return smartUpdatingData;
		case SmartModel.State.UNAVAILABLE:
			return smartUnavailableData;
		case SmartModel.State.UNUSABLE:
			return smartUnusableData;
		case SmartModel.State.NO_PROVISIONING:
		case SmartModel.State.NO_PERSONALIZATION:
			return smartNotSetupData;
		default:
			return empty;
		}
	}

	SuggestionData {
		id: updateCheckError

		animationSymbol: Symbol.Type.ERROR

		//: MOBILE
		continueButtonText: qsTr("Back")
		text: SmartModel.errorString
		//: MOBILE
		title: qsTr("Smart-eID check failed")

		onContinueClicked: root.cancelClicked()
	}
	SuggestionData {
		id: empty

		//: MOBILE
		text: qsTr("Unknown result code: %1").arg(root.result)
	}
	SuggestionData {
		id: smartUpdatingData

		animationType: AnimationLoader.Type.HOURGLASS

		//: MOBILE
		text: qsTr("Please wait a moment.")

		//: MOBILE
		title: qsTr("Updating Smart-eID status...")
	}
	SuggestionData {
		id: smartUnavailableData

		animationSymbol: Symbol.Type.ERROR
		continueButtonIcon: "qrc:///images/device_button.svg"
		//: MOBILE
		continueButtonText: qsTr("Check device and ID card")
		//: MOBILE
		text: qsTr("Your mobile device does not meet the technical requirements for Smart-eID.<br><br>You may check if your device and ID card are suitable to use the eID function.")

		//: MOBILE
		title: qsTr("Smart-eID not supported")

		onContinueClicked: root.checkDevice()
	}
	SuggestionData {
		id: smartUnusableData

		continueButtonIcon: "qrc:///images/identify.svg"
		//: MOBILE
		text: "<style>ul{-qt-list-indent: 0;}li{margin-top:1em;}</style>" +
		//: MOBILE
		qsTr("Your device meets the technical requirements for Smart-eID, however, the Smart-eID that was set up is invalid.") + "<br><br>" +
		//: MOBILE
		qsTr("Possible causes are:") + "<ul><li>" +
		//: MOBILE
		qsTr("The setup has not been completed.") + "</li><li>" +
		//: MOBILE
		qsTr("The Smart-eID PIN has been entered incorrectly 3 times.") + "</li><li>" +
		//: MOBILE
		qsTr("The %1 has been uninstalled temporarily.").arg(Qt.application.name) + "</li></ul>" + "<br>" +
		//: MOBILE
		qsTr("You may continue with the setup of the Smart-eID.")
		textFormat: Text.RichText

		//: MOBILE
		title: qsTr("Smart-eID invalid")

		onContinueClicked: root.runSmartSetup()
	}
	SuggestionData {
		id: smartNotSetupData

		continueButtonIcon: "qrc:///images/identify.svg"

		//: MOBILE
		text: qsTr("Your device meets the technical requirements for Smart-eID. You may now continue the setup process.")

		//: MOBILE
		title: qsTr("Smart-eID supported")

		onContinueClicked: root.runSmartSetup()
	}
}
