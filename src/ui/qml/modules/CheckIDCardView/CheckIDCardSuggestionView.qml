/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.CheckResultView
import Governikus.Type

CheckResultSuggestionView {
	id: root

	required property int result
	property bool usedInOnboarding: false

	signal checkSuccess
	signal restartCheck

	Accessible.name: suggestionData.title
	suggestionData: {
		switch (result) {
		case CheckIDCardModel.Result.UNKNOWN_CARD_DETECTED:
			return unknownCardSuggestionData;
		case CheckIDCardModel.Result.INSUFFICIENT_APDU_LENGTH:
			return insufficientApduLengthSuggestionData;
		case CheckIDCardModel.Result.CARD_ACCESS_FAILED:
			return cardAccessFailedSuggestionData;
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

		//: LABEL ALL_PLATFORMS
		text: qsTr("Unknown result code: %1").arg(root.result)
	}
	SuggestionData {
		id: unknownCardSuggestionData

		continueButtonIcon: "qrc:///images/device_button.svg"
		//: LABEL ALL_PLATFORMS
		continueButtonText: qsTr("Retry")
		//: LABEL ALL_PLATFORMS
		hintText: qsTr("Try a different card position, make sure that you do not move the ID card during the check and that there are no other NFC cards (e.g. credit cards) near the device.")
		//: LABEL ALL_PLATFORMS
		text: qsTr("No supported ID card was detected. The %1 supports:<p><ul><li>German ID cards</li><li>Electronic residence permits (eAT)</li><li>eID cards for citizen of the EU/EEA/</li></ul></p>If you have used one of the above documents and this error message still appears, please restart the check.").arg(Qt.application.name)

		//: LABEL ALL_PLATFORMS
		title: qsTr("No supported card detected")

		onContinueClicked: root.restartCheck()
	}
	SuggestionData {
		id: insufficientApduLengthSuggestionData

		continueButtonIcon: "qrc:///images/open_website.svg"
		//: LABEL ALL_PLATFORMS
		continueButtonText: qsTr("Open website")
		//: LABEL ALL_PLATFORMS
		text: qsTr("The NFC interface of your mobile device does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.<br><br>You can find smartphones compatible with the %1 on our website.").arg(Qt.application.name)

		//: LABEL ALL_PLATFORMS
		title: qsTr("No extended length")

		onContinueClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language))
	}
	SuggestionData {
		id: cardAccessFailedSuggestionData

		readonly property string deviceUrl: "https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language)

		continueButtonIcon: "qrc:///images/device_button.svg"

		//: LABEL ALL_PLATFORMS
		continueButtonText: qsTr("Retry")
		//: LABEL ALL_PLATFORMS
		text: qsTr("It was not possible to establish a stable connection with your ID card.<br><br>Please start the check again. Try a different card position and make sure not to move the card during the test.<br><br>If a connection to the ID card cannot be established even with different card positions, this indicates that the NFC interface of your mobile device cannot supply the ID card with sufficient power.<br><br>Smartphones compatible with %1 can be found on our <a href=\"%2\">website</a>.").arg(Qt.application.name).arg(deviceUrl)
		//: LABEL ALL_PLATFORMS
		title: qsTr("ID card access failed")

		onContinueClicked: root.restartCheck()
	}
	SuggestionData {
		id: pinSuspendedSuggestionData

		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ALL_PLATFORMS Sentence 1 of 3 of CAN explanation
		text: qsTr("The ID card PIN has been entered incorrectly 2 times in a row. This is why you must first enter the 6-digit Card Access Number (CAN) for the next identification process. You can find it at the bottom right of the front of your ID card.") + "<br><br>" +
		//: LABEL ALL_PLATFORMS Sentence 2 of 3 of CAN explanation
		(root.usedInOnboarding ? qsTr("You may continue the onboarding and change your PIN.") :
			//: LABEL ALL_PLATFORMS Sentence 2 of 3 of CAN explanation
			qsTr("You may now try the function: \"See my personal data\".")) + " " +
		//: LABEL ALL_PLATFORMS Sentence 3 of 3 of CAN explanation
		qsTr("There, you can also use the CAN to unblock the ID card PIN.")

		//: LABEL ALL_PLATFORMS
		title: qsTr("ID card PIN suspended")

		onContinueClicked: root.checkSuccess()
	}
	SuggestionData {
		id: pinBlockedSuggestionData

		continueButtonIcon: "qrc:///images/identify.svg"
		hintButtonText: PinResetInformationModel.pinResetActionText
		hintText: PinResetInformationModel.noPinAndNoPukHint
		//: LABEL ALL_PLATFORMS Sentence 1 of 3 of PUK explanation
		text: qsTr("The ID card PIN has been entered incorrectly 3 times. Therefore, you must first enter the 10-digit PUK during the next authentication process. You can find it in the PIN letter you received after applying for your ID card.") + "<br><br>" +
		//: LABEL ALL_PLATFORMS Sentence 2 of 3 of PUK explanation
		(root.usedInOnboarding ? qsTr("You may continue the onboarding and change your PIN.") :
			//: LABEL ALL_PLATFORMS Sentence 2 of 3 of PUK explanation
			qsTr("You may now try the function: \"See my personal data\".")) + " " +
		//: LABEL ALL_PLATFORMS Sentence 3 of 3 of PUK explanation
		qsTr("Have your PUK ready to unlock the ID card PIN.")

		//: LABEL ALL_PLATFORMS
		title: qsTr("ID card PIN blocked")

		onContinueClicked: root.checkSuccess()
		onHintClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}
}
