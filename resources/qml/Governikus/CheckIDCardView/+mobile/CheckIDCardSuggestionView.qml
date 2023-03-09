/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.CheckResultView 1.0
import Governikus.Type.CheckIDCardModel 1.0
import Governikus.Type.PinResetInformationModel 1.0
import Governikus.Type.SettingsModel 1.0

CheckResultSuggestionView {
	id: root

	property int result

	signal restartCheck
	signal startAuth

	Accessible.name: suggestionData.title
	suggestionData: {
		switch (result) {
		case CheckIDCardModel.NO_NFC:
			return noNfcSuggestionData;
		case CheckIDCardModel.UNKNOWN_CARD_DETECTED:
			return unknownCardSuggestionData;
		case CheckIDCardModel.INSUFFICIENT_APDU_LENGTH:
			return insufficientApduLengthSuggestionData;
		case CheckIDCardModel.CARD_ACCESS_FAILED:
			return cardAccessFailedSuggestionData;
		case CheckIDCardModel.PIN_DEACTIVATED:
			return pinDeactivatedSuggestionData;
		case CheckIDCardModel.PIN_SUSPENDED:
			return pinSuspendedSuggestionData;
		case CheckIDCardModel.PIN_BLOCKED:
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
		continueButtonIcon: "qrc:///images/material_open_in_new.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Open website")
		//: LABEL ANDROID IOS
		text: qsTr("Your mobile device has no NFC interface. This is required to read the ID card. However, you can use a separate smartphone as card reader to utilize the eID function.<br><br>You can find smartphones compatible with the %1 on our website.").arg(Qt.application.name)

		//: LABEL ANDROID IOS
		title: qsTr("No NFC")

		onContinueClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language))
	}
	SuggestionData {
		id: unknownCardSuggestionData
		continueButtonIcon: "qrc:///images/mobile/device.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Retry")
		//: LABEL ANDROID IOS
		text: qsTr("No supported ID card was detected. The %1 supports:<p><ul><li>German ID cards</li><li>Electronic residence permits (eAT)</li><li>eID cards</li></ul></p>If you have used one of the above documents and this error message still appears, please restart the check.<br><br>Tip: Try a different card position, make sure that you do not move the ID card during the check and that there are no other NFC cards (e.g. credit cards) near the device.").arg(Qt.application.name)

		//: LABEL ANDROID IOS
		title: qsTr("No supported card detected")

		onContinueClicked: restartCheck()
	}
	SuggestionData {
		id: insufficientApduLengthSuggestionData
		continueButtonIcon: "qrc:///images/material_open_in_new.svg"
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

		continueButtonIcon: "qrc:///images/mobile/device.svg"

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
		text: PinResetInformationModel.activateOnlineFunctionDescription

		//: LABEL ANDROID IOS
		title: qsTr("Online identification feature disabled")

		onContinueClicked: navigationAction.clicked()
		onHintClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}
	SuggestionData {
		id: pinSuspendedSuggestionData
		continueButtonIcon: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		continueButtonText: qsTr("Continue")
		//: LABEL ANDROID IOS
		text: qsTr("The ID card PIN has been entered incorrectly twice in a row. This is why you must first enter the six-digit Card Access Number (CAN) for the next identification process. You can find it at the bottom right of the front of your ID card.<br><br>You may now try the function: \"See my personal data\". There you can also use the CAN to unblock the ID card PIN.")

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
		text: qsTr("The ID card PIN has been entered incorrectly thrice. Therefore, you must first enter the ten-digit PUK during the next authentication process. You can find it in the PIN letter you received after applying for your ID card.<br><br>You may now try the function: \"See my personal data\". Have your PUK ready to unlock the ID card PIN.")

		//: LABEL ANDROID IOS
		title: qsTr("ID card PIN blocked")

		onContinueClicked: startAuth()
		onHintClicked: Qt.openUrlExternally(PinResetInformationModel.pinResetUrl)
	}
}
