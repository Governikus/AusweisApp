/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.CheckResultView
import Governikus.Type

CheckResultSuggestionView {
	id: root

	required property int result

	signal restartCheck
	signal startAuth

	Accessible.name: suggestionData.title
	suggestionData: {
		switch (result) {
		case CheckIDCardModel.Result.NO_NFC:
			return noNfcSuggestionData;
		case CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED:
			return unknownCardSuggestionData;
		case CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH:
			return insufficientApduLengthSuggestionData;
		case CheckIDCardModel.Result.CARD_ACCESS_FAILED:
			return cardAccessFailedSuggestionData;
		case CheckIDCardModel.Result.PIN_DEACTIVATED:
			return pinDeactivatedSuggestionData;
		case CheckIDCardModel.Result.PIN_SUSPENDED:
			return pinSuspendedSuggestionData;
		case CheckIDCardModel.Result.PIN_BLOCKED:
			return pinBlockedSuggestionData;
		default:
			return empty;
		}
	}

	SuggestionData {
		id: empty

		//: LABEL ANDROID IOS
		text: qsTr("Unknown result code: %1").arg(result)
	}
	SuggestionData {
		id: noNfcSuggestionData

		continueButtonIcon: "qrc:///images/open_website.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("List of compatible devices")
		//: LABEL ANDROID IOS
		text: qsTr("Your mobile device has no NFC interface. This is required to read the ID card. To use the eID function on this device you may pair a NFC capable smartphone as a card reader.<br><br>To pair a smartphone go to <b>Settings</b> and <b>Manage Pairings</b>.<br><br>You can find smartphones compatible with the %1 on our website.").arg(Qt.application.name)

		//: LABEL ANDROID IOS
		title: qsTr("No NFC")

		onContinueClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language))
	}
	SuggestionData {
		id: unknownCardSuggestionData

		continueButtonIcon: "qrc:///images/mobile/device_button.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Retry")
		//: LABEL ANDROID IOS
		hintText: qsTr("Try a different card position, make sure that you do not move the ID card during the check and that there are no other NFC cards (e.g. credit cards) near the device.")
		//: LABEL ANDROID IOS
		text: qsTr("No supported ID card was detected. The %1 supports:<p><ul><li>German ID cards</li><li>Electronic residence permits (eAT)</li><li>eID cards for citizen of the EU/EEA/</li></ul></p>If you have used one of the above documents and this error message still appears, please restart the check.").arg(Qt.application.name)

		//: LABEL ANDROID IOS
		title: qsTr("No supported card detected")

		onContinueClicked: restartCheck()
	}
	SuggestionData {
		id: insufficientApduLengthSuggestionData

		continueButtonIcon: "qrc:///images/open_website.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Open website")
		//: LABEL ANDROID IOS
		text: qsTr("The NFC interface of your mobile device does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.<br><br>You can find smartphones compatible with the %1 on our website.").arg(Qt.application.name)

		//: LABEL ANDROID IOS
		title: qsTr("No extended length")

		onContinueClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language))
	}
	SuggestionData {
		id: cardAccessFailedSuggestionData

		readonly property string deviceUrl: "https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language)

		continueButtonIcon: "qrc:///images/mobile/device_button.svg"

		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Retry")
		//: LABEL ANDROID IOS
		text: qsTr("It was not possible to establish a stable connection with your ID card.<br><br>Please start the check again. Try a different card position and make sure not to move the card during the test.<br><br>If a connection to the ID card cannot be established even with different card positions, this indicates that the NFC interface of your mobile device cannot supply the ID card with sufficient power.<br><br>Smartphones compatible with %1 can be found on our <a href=\"%2\">website</a>.").arg(Qt.application.name).arg(deviceUrl)
		//: LABEL ANDROID IOS
		title: qsTr("ID card access failed")

		onContinueClicked: restartCheck()
	}
	SuggestionData {
		id: pinDeactivatedSuggestionData

		//: LABEL ANDROID IOS
		continueButtonText: qsTr("OK")
		hintButtonText: PinResetInformationModel.pinResetActionText
		hintText: PinResetInformationModel.activateOnlineFunctionHint
		//: LABEL ANDROID IOS Hint when a workflow failed because the eID function was not activated
		hintTitle: qsTr("Activate the eID function.")
		text: PinResetInformationModel.activateOnlineFunctionDescription

		//: LABEL ANDROID IOS
		title: qsTr("eID function disabled")

		onContinueClicked: navigationAction.clicked()
		onHintClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}
	SuggestionData {
		id: pinSuspendedSuggestionData

		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Continue")
		//: LABEL ANDROID IOS
		text: qsTr("The ID card PIN has been entered incorrectly 2 times in a row. This is why you must first enter the 6-digit Card Access Number (CAN) for the next identification process. You can find it at the bottom right of the front of your ID card.<br><br>You may now try the function: \"See my personal data\". There you can also use the CAN to unblock the ID card PIN.")

		//: LABEL ANDROID IOS
		title: qsTr("ID card PIN suspended")

		onContinueClicked: startAuth()
	}
	SuggestionData {
		id: pinBlockedSuggestionData

		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Continue")
		hintButtonText: PinResetInformationModel.pinResetActionText
		hintText: PinResetInformationModel.noPinAndNoPukHint
		//: LABEL ANDROID IOS
		text: qsTr("The ID card PIN has been entered incorrectly 3 times. Therefore, you must first enter the 10-digit PUK during the next authentication process. You can find it in the PIN letter you received after applying for your ID card.<br><br>You may now try the function: \"See my personal data\". Have your PUK ready to unlock the ID card PIN.")

		//: LABEL ANDROID IOS
		title: qsTr("ID card PIN blocked")

		onContinueClicked: startAuth()
		onHintClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}
}
