/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.CheckResultView 1.0
import Governikus.Style 1.0
import Governikus.Type.SmartModel 1.0

CheckResultSuggestionView {
	id: root

	property int result

	signal checkDevice
	signal runSmartSetup

	Accessible.name: suggestionData.title
	suggestionData: {
		switch (result) {
		case SmartModel.SMART_UPDATING_STATUS:
			return smartUpdatingData;
		case SmartModel.SMART_UNAVAILABLE:
			return smartUnvailableData;
		case SmartModel.SMART_UNUSABLE:
			return smartUnsuableData;
		case SmartModel.SMART_NO_PROVISIONING:
			return noProvsioningData;
		case SmartModel.SMART_NO_PERSONALIZATION:
			return noPersonalizationData;
		default:
			return empty;
		}
	}
	titleBarColor: Style.color.accent_smart

	SuggestionData {
		id: empty

		//: LABEL ANDROID IOS
		text: qsTr("Unknown result code: %1").arg(result)
	}
	SuggestionData {
		id: smartUpdatingData
		//: LABEL ANDROID IOS
		text: qsTr("Please wait a moment.")

		//: LABEL ANDROID IOS
		title: qsTr("Updating Smart-eID status...")
	}
	SuggestionData {
		id: smartUnvailableData
		continueButtonIcon: "qrc:///images/mobile/device.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Check device and ID card")
		//: LABEL ANDROID IOS
		text: qsTr("Your mobile device does not meet the technical requirements for Smart-eID.<br><br>You may check if your device and ID card are suitable to use the eID functionality.")

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID not supported")

		onContinueClicked: checkDevice()
	}
	SuggestionData {
		id: smartUnsuableData
		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Set up Smart-eID")
		//: LABEL ANDROID IOS
		text: "<style>ul{-qt-list-indent: 0;}li{margin-top:1em;}</style>" +
		//: LABEL ANDROID IOS
		qsTr("Your device meets the technical requirements for Smart-eID, however, the Smart-eID that was set up is invalid.") + "<br><br>" +
		//: LABEL ANDROID IOS
		qsTr("Possible causes are:") + "<ul><li>" +
		//: LABEL ANDROID IOS
		qsTr("The setup has not been completed.") + "</li><li>" +
		//: LABEL ANDROID IOS
		qsTr("The preparation for the Smart-eID is defective.") + "</li><li>" +
		//: LABEL ANDROID IOS
		qsTr("The Smart-eID PIN has been entered incorrectly three times.") + "</li><li>" +
		//: LABEL ANDROID IOS
		qsTr("The AusweisApp2 has been uninstalled temporarily.") + "</li></ul>" + "<br>" +
		//: LABEL ANDROID IOS
		qsTr("Please restart the setup of the Smart-eID.")
		textFormat: Text.RichText

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID invalid")

		onContinueClicked: runSmartSetup()
	}
	SuggestionData {
		id: noProvsioningData
		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Set up Smart-eID")
		//: LABEL ANDROID IOS
		text: qsTr("Your device meets the technical requirements for Smart-eID, but is not yet provisioned for setup. The provisioning is done automatically during the Smart-eID setup process.")

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID not prepared")

		onContinueClicked: runSmartSetup()
	}
	SuggestionData {
		id: noPersonalizationData
		continueButtonIcon: "qrc:///images/identify.svg"

		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Set up Smart-eID")
		//: LABEL ANDROID IOS
		text: qsTr("Your device meets the technical requirements for Smart-eID and is already provisioned for setup. You can now start the Smart-eID setup.")

		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID not set up")

		onContinueClicked: runSmartSetup()
	}
}
