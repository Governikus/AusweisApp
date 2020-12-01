/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.ResultView 1.0
import Governikus.Type.CheckIDCardModel 1.0
import Governikus.Type.SettingsModel 1.0

ResultView {
	id: root

	signal restartCheck()

	property int result
	readonly property var suggestionData: {
		switch(result) {
			case CheckIDCardModel.NO_NFC:
				return noNfcSuggestionData
			case CheckIDCardModel.UNKNOWN_CARD_DETECTED:
				return unknownCardSuggestionData
			case CheckIDCardModel.INSUFFICIENT_APDU_LENGTH:
				return insufficientApduLengthSuggestionData
			case CheckIDCardModel.CARD_ACCESS_FAILED:
				return cardAccessFailedSuggestionData
			case CheckIDCardModel.PIN_DEACTIVATED:
				return pinDeactivatedSuggestionData
			case CheckIDCardModel.PIN_SUSPENDED:
				return pinSuspendedSuggestionData
			case CheckIDCardModel.PIN_BLOCKED:
				return pinBlockedSuggestionData
			default:
				return undefined
		}
	}

	Accessible.name: suggestionData.title
	//: LABEL ANDROID IOS
	Accessible.description: qsTr("This is the suggestion page of the device and ID card check of the AusweisApp2.")

	navigationAction: NavigationAction {
		state: "cancel"
		onClicked: {
			navBar.lockedAndHidden = false
			firePopAll()
		}
	}

	title: suggestionData.title
	resultType: ResultView.Type.IsInfo
	text: suggestionData.text
	buttonLeft.text: suggestionData.buttonTitle !== undefined ? suggestionData.buttonTitle : qsTr("Open website")
	buttonLeft.icon.source: suggestionData.buttonIcon !== undefined ? suggestionData.buttonIcon : "qrc:///images/material_open_in_new.svg"
	buttonLeft.tintIcon: true

	onClicked: suggestionData.onButtonClicked !== undefined ? suggestionData.onButtonClicked() : Qt.openUrlExternally(suggestionData.suggestionUrl)

	function startIdentify() {
		// We need to keep a reference to the navbar since after calling firePopAll() 'navBar' becomes undefined
		let navigation = navBar
		firePopAll()
		navigation.showIdentify()
	}

	QtObject {
		id: noNfcSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("No NFC")

		//: LABEL ANDROID IOS
		readonly property string text: qsTr("Your mobile device has no NFC interface. This is required to read the ID card.<br><br>You can find smartphones compatible with the %1 on our website.")
			.arg(Qt.application.name)

		readonly property string suggestionUrl: "https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language)
	}

	QtObject {
		id: unknownCardSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("No supported card detected")

		//: LABEL ANDROID IOS
		readonly property string text: qsTr("No supported ID card was detected. The %1 supports:<p><ul><li>German ID cards</li><li>Electronic residence permit (eAT)</li><li>Union citizen cards</li></ul></p>If you have used one of the above documents and this error message still appears, please restart the check.<br><br>Tip: Try a different card position, make sure that you do not move the ID card during the check and that there are no other NFC cards (e.g. credit cards) near the device.")
			.arg(Qt.application.name)

		//: LABEL ANDROID IOS
		readonly property string buttonTitle: qsTr("Retry")
		readonly property string buttonIcon: "qrc:///images/mobile/device.svg"

		function onButtonClicked() {
			restartCheck()
		}
	}

	QtObject {
		id: insufficientApduLengthSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("No extended length")

		//: LABEL ANDROID IOS

		readonly property string text: qsTr("The NFC interface of your mobile device does not support Extended Length communication and cannot be used to read the ID card. Unfortunately, the %1 has no influence on this restriction.<br><br>You can find smartphones compatible with the %1 on our website.")
			.arg(Qt.application.name)

		readonly property string suggestionUrl: "https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language)
	}

	QtObject {
		id: cardAccessFailedSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("ID card access failed")

		readonly property string suggestionUrl: "https://www.ausweisapp.bund.de/%1/aa2/mobile-devices".arg(SettingsModel.language)

		//: LABEL ANDROID IOS
		readonly property string text: qsTr("It was not possible to establish a stable connection with your ID card.<br><br>Please start the check again. Try a different card position and make sure not to move the card during the test.<br><br>If a connection to the ID card cannot be established even with different card positions, this indicates that the NFC interface of your mobile device cannot supply the ID card with sufficient power.<br><br>Smartphones compatible with %1 can be found on our <a href=\"%2\">website</a>.")
			.arg(Qt.application.name)
			.arg(suggestionUrl)

		//: LABEL ANDROID IOS
		readonly property string buttonTitle: qsTr("Retry")
		readonly property string buttonIcon: "qrc:///images/mobile/device.svg"

		function onButtonClicked() {
			restartCheck()
		}
	}

	QtObject {
		id: pinDeactivatedSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("Online identification feature disabled")

		//: LABEL ANDROID IOS
		readonly property string text: qsTr("According to your request when picking up your ID card, the online identification function is not activated.<br><br>Please contact the relevant authorities to activate the online identification function.<br><br>For further information, please visit the ID card portal.")

		readonly property string suggestionUrl: "https://www.personalausweisportal.de"
	}

	QtObject {
		id: pinSuspendedSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("PIN suspended")

		//: LABEL ANDROID IOS
		readonly property string text: qsTr("The PIN has been entered incorrectly twice in a row. Before the third attempt, the six-digit Card Access Number (CAN) must be entered. You can find it at the bottom right of the front of your ID card.<br><br>You may now try the function: \"See my personal data\". There you can also use the CAN to resume the PIN.")

		//: LABEL ANDROID IOS
		readonly property string buttonTitle: qsTr("Continue")
		readonly property string buttonIcon: "qrc:///images/identify.svg"

		function onButtonClicked() {
			startIdentify()
		}
	}

	QtObject {
		id: pinBlockedSuggestionData

		//: LABEL ANDROID IOS
		readonly property string title: qsTr("PIN blocked")

		//: LABEL ANDROID IOS
		readonly property string text: qsTr("The PIN has been entered incorrectly thrice. Before the next attempt, the ten-digit Personal Unblocking Key (PUK) must be entered. You can find it in the PIN letter you received after applying for your ID card.<br><br>You may now try the function: \"See my personal data\". There you can also use the PUK to unblock the PIN again.<br><br>Please note: The PUK can only be used ten times to unblock the PIN. After that, please contact the responsible authority to unblock your PIN again.")

		//: LABEL ANDROID IOS
		readonly property string buttonTitle: qsTr("Continue")
		readonly property string buttonIcon: "qrc:///images/identify.svg"

		function onButtonClicked() {
			startIdentify()
		}
	}
}
