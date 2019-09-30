/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: root

	signal close()

	property int passwordType: NumberModel.passwordType

	Accessible.name: qsTr("Password information") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the password information section of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: close()
	Keys.onEnterPressed: close()
	Keys.onEscapePressed: close()

	titleBarAction: TitleBarAction {
		rootEnabled: false
		showSettings: false
		text: d.passwordString
	}

	QtObject {
		id: d

		readonly property string passwordString: passwordType === NumberModel.PASSWORD_PIN ? "PIN"
											   : passwordType === NumberModel.PASSWORD_CAN ? "CAN"
											   : passwordType === NumberModel.PASSWORD_PUK ? "PUK"
											   : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? "Remote PIN"
											   : "UNKNOWN"
	}

	GText {
		id: headline

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.pane_padding

		activeFocusOnTab: true
		Accessible.name: headline.text

		wrapMode: Text.WordWrap
		//: %1 can be "PIN, CAN, PUK or UNKNOWN"
		text: qsTr("%1 information").arg(d.passwordString) + SettingsModel.translationTrigger
		textStyle: Style.text.header
		FocusFrame {
			dynamic: false
		}
	}

	GText {
		id: infoText

		anchors.top: headline.bottom
		anchors.left: parent.left
		anchors.right: infoImage.left
		anchors.margins: Constants.pane_padding

		activeFocusOnTab: true
		Accessible.name: infoText.text

		text: {
			SettingsModel.translationTrigger

			if (passwordType === NumberModel.PASSWORD_CAN) {
				//: INFO DESKTOP_QML Description text of CAN
				return qsTr("The card access number (CAN) is only required if you have already entered the PIN incorrectly twice. In order to prevent a third incorrect entry and thus the blocking of the ID card without your consent, the CAN is also requested at this point. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")
			}
			if (passwordType === NumberModel.PASSWORD_PUK) {
				//: INFO DESKTOP_QML Description text of PUK
				return qsTr("The personal unblocking key (PUK) is only required if you entered the the wrong PIN three times. The online eID function is blocked at this time. The PUK is a ten-digit number you received with the letter sent to you by your competent authority (marked in red). Please note that you can only use the PUK to unblock the PIN entry. If you have forgotten your PIN, you can have a new PIN set at your competent authority.")
			}
			if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				//: INFO DESKTOP_QML Description text of SaC pairing
				return qsTr("You may use your smartphone as a card reader with AusweisApp2. The smartphone needs to feature a supported NFC chipset and both devices, your smartphone and this machine, need to be connected to the same WiFi network.<br><br>Please make sure that the remote service is running on your smartphone. Start the pairing process by clicking the \"Start pairing\" button and enter the shown 4-digit PIN.")
			}

			//: INFO DESKTOP_QML Description text of PIN
			return qsTr("The personal identification number (PIN) is required for every use of the online eID function. You can change it anytime and indefinitely if you know your valid PIN. For your 6-digit PIN choose a combination of numbers, that is not easy to guess, neither \"123456\" nor your birth date, or any other numbers printed on the ID card. If you are no longer aware of your valid PIN, you will need to contact the authority responsible for issuing your identification document to renew your PIN.\n\nWhen changing the PIN for the first time, please use your 5-digit transport PIN. You will find the transport PIN in the letter you received from the authority responsible for issuing your identification document (marked in red) after you have applied for your identity card.\n\nPlease note that you can not use the online eID function with your 5-digit transport PIN. A change to a 6-digit PIN is mandatory.")
		}
		textStyle: Style.text.header

		FocusFrame {
			dynamic: false
		}
	}

	Image {
		id: infoImage

		sourceSize.width: (passwordType === NumberModel.PASSWORD_CAN ? 800 : 560) * ApplicationModel.scaleFactor
		anchors.top: headline.bottom
		anchors.right: parent.right
		anchors.margins: Constants.pane_padding

		verticalAlignment: Image.AlignTop
		fillMode: Image.PreserveAspectFit
		source: passwordType === NumberModel.PASSWORD_CAN ? "qrc:///images/desktop/id_card.png"
			  : passwordType === NumberModel.PASSWORD_PUK ? "qrc:///images/desktop/pin-letter-page2.png"
			  : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? "qrc:///images/phone_to_pc.svg"
			  : "qrc:///images/desktop/pin-letter-page1.png"

		Rectangle {
			visible: passwordType === NumberModel.PASSWORD_CAN
			height: 50 * ApplicationModel.scaleFactor
			width: 160 * ApplicationModel.scaleFactor
			anchors.top: parent.top
			anchors.topMargin: 333 * ApplicationModel.scaleFactor
			anchors.left: parent.left
			anchors.leftMargin: 598 * ApplicationModel.scaleFactor

			color: Style.color.transparent
			border.color: Constants.red
			border.width: Math.max(1, 6 * ApplicationModel.scaleFactor)
		}
	}

	NavigationButton {
		id: button

		anchors {
			margins: Constants.component_spacing
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		buttonType: Qt.BackButton
		onClicked: root.close()
	}
}
