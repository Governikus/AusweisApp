/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.CheckResultView
import Governikus.ResultView
import Governikus.Style
import Governikus.Type.SmartModel

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
		case SmartModel.SMART_UPDATING_STATUS:
			return smartUpdatingData;
		case SmartModel.SMART_UNAVAILABLE:
			return smartUnvailableData;
		case SmartModel.SMART_UNUSABLE:
			return smartUnusableData;
		case SmartModel.SMART_NO_PROVISIONING:
		case SmartModel.SMART_NO_PERSONALIZATION:
			return smartNotSetupData;
		default:
			return empty;
		}
	}

	SuggestionData {
		id: updateCheckError

		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Back")
		icon: "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name)
		text: SmartModel.errorString
		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID check failed")

		onContinueClicked: cancelClicked()
	}
	SuggestionData {
		id: empty

		//: LABEL ANDROID IOS
		text: qsTr("Unknown result code: %1").arg(result)
	}
	SuggestionData {
		id: smartUpdatingData

		icon: "qrc:///animations/hourglass.svg"
		//: LABEL ANDROID IOS
		text: qsTr("Please wait a moment.")

		//: LABEL ANDROID IOS
		title: qsTr("Updating Smart-eID status...")
	}
	SuggestionData {
		id: smartUnvailableData

		continueButtonIcon: "qrc:///images/mobile/device_button.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Check device and ID card")
		icon: "qrc:///images/status_error_%1.svg".arg(Style.currentTheme.name)
		//: LABEL ANDROID IOS
		text: qsTr("Your mobile device does not meet the technical requirements for Smart-eID.<br><br>You may check if your device and ID card are suitable to use the eID function.")

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID not supported")

		onContinueClicked: checkDevice()
	}
	SuggestionData {
		id: smartUnusableData

		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Continue")
		//: LABEL ANDROID IOS
		text: "<style>ul{-qt-list-indent: 0;}li{margin-top:1em;}</style>" +
		//: LABEL ANDROID IOS
		qsTr("Your device meets the technical requirements for Smart-eID, however, the Smart-eID that was set up is invalid.") + "<br><br>" +
		//: LABEL ANDROID IOS
		qsTr("Possible causes are:") + "<ul><li>" +
		//: LABEL ANDROID IOS
		qsTr("The setup has not been completed.") + "</li><li>" +
		//: LABEL ANDROID IOS
		qsTr("The Smart-eID PIN has been entered incorrectly 3 times.") + "</li><li>" +
		//: LABEL ANDROID IOS
		qsTr("The %1 has been uninstalled temporarily.").arg(Qt.application.name) + "</li></ul>" + "<br>" +
		//: LABEL ANDROID IOS
		qsTr("You may continue with the setup of the Smart-eID.")
		textFormat: Text.RichText

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID invalid")

		onContinueClicked: runSmartSetup()
	}
	SuggestionData {
		id: smartNotSetupData

		continueButtonIcon: "qrc:///images/identify.svg"

		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Continue")
		//: LABEL ANDROID IOS
		text: qsTr("Your device meets the technical requirements for Smart-eID. You may now continue the setup process.")

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID supported")

		onContinueClicked: runSmartSetup()
	}
}
