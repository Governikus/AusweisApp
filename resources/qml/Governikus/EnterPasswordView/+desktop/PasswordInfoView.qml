/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
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
		rootEnabled: ApplicationModel.currentWorkflow === ""
		showSettings: false
		text: headline.text
		showHelp: false
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
		//: LABEL DESKTOP_QML
		text: (passwordType === NumberModel.PASSWORD_CAN ? qsTr("CAN information")
			 //: LABEL DESKTOP_QML
		     : passwordType === NumberModel.PASSWORD_PUK ? qsTr("PUK information")
			 //: LABEL DESKTOP_QML
		     : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? qsTr("Smartphone as card reader information")
			 //: LABEL DESKTOP_QML
			 : qsTr("PIN information")) + SettingsModel.translationTrigger
		textStyle: Style.text.header_inverse
		FocusFrame {}
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

			if (passwordType === NumberModel.PASSWORD_CAN && NumberModel.isCanAllowedMode) {
				//: INFO DESKTOP_QML Description text of CAN-allowed authentication
				return qsTr("The Card Access Number (CAN) allows to access the imprinted data of the ID card. In order to allow on-site reading of the personal data the service provider needs to acquire governmental authorization to do so. On-site reading is usually employed to automatically fill forms and prevent spelling mistakes when transfering the personal data.")
			}
			if (passwordType === NumberModel.PASSWORD_CAN && !NumberModel.isCanAllowedMode) {
				//: INFO DESKTOP_QML Description text of CAN if required for third PIN attempt
				return qsTr("The Card Access Number (CAN) is required if the PIN has already been entered incorrectly twice. In order to prevent a third incorrect entry and thus the blocking of the PIN without your consent, the CAN is also requested at this point. The CAN is a six-digit number that can be found on the front of the ID card. It is located at the bottom right next to the validity date (marked in red).")
			}
			if (passwordType === NumberModel.PASSWORD_PUK) {
				//: INFO DESKTOP_QML Description text of PUK
				return qsTr("The Personal Unblocking Key (PUK) is required if the PIN has been entered three times. At this point the PIN is blocked. The PUK is a ten-digit number you received with the letter sent to you by your competent authority (marked in red). Please note that you can only use the PUK to unblock the PIN entry. If you have forgotten your PIN, you can have a new PIN set at your competent authority.")
			}
			if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				//: INFO DESKTOP_QML Description text of SaC pairing
				return qsTr("You may use your smartphone as a card reader with AusweisApp2. The smartphone needs to feature a supported NFC chipset and both devices, your smartphone and this machine, need to be connected to the same WiFi network.<br><br>To use your smartphone as a card reader you'll always need to activate the remote service in the AusweisApp2 on the smartphone. For the first time you'll also need to start the pairing mode on your smartphone, select the device from the list of available devices on this machine and then enter the pairing code shown on the phone.")
			}

			//: INFO DESKTOP_QML Description text of PIN
			return qsTr("The Personal Identification Number (PIN) is chosen by you and is required for every use of the online eID function. You can change it anytime and indefinitely if you know your valid PIN. For your six-digit PIN choose a combination of numbers, that is not easy to guess, neither \"123456\" nor your birth date, or any other numbers printed on the ID card. If you are no longer aware of your valid PIN, you will need to contact your responsible authority to renew your PIN.<br><br>When changing the PIN for the first time, please use the five-digit Transport PIN. You will find the Transport PIN in the letter you received from your responsible authority (marked in red) after you have applied for your identity card.<br><br>Please note that you can not use the online eID function with the five-digit Transport PIN. A change to a six-digit PIN is mandatory.")
		}
		textStyle: Style.text.header_inverse
		horizontalAlignment: Text.AlignJustify

		FocusFrame {}
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
				: passwordType === NumberModel.PASSWORD_REMOTE_PIN ? "qrc:///images/phone_to_pc.svg"
				: "qrc:///images/desktop/pin_letter_pinpuk_same_page.svg"

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

		GText {
			visible: passwordType === NumberModel.PASSWORD_PIN || passwordType === NumberModel.PASSWORD_PUK

			anchors.top: parent.bottom
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.topMargin: Constants.component_spacing

			textStyle: Style.text.normal_inverse
			text: (imageChangeTimer.alternativeLetter ?
				   //: LABEL DESKTOP_QML
				   qsTr("PIN/PUK on different pages") :
				   //: LABEL DESKTOP_QML
				   qsTr("PIN/PUK on the same page")
				  ) + SettingsModel.translationTrigger
		}
	}

	states: State {
		name: "alternativePinLetter"
		when: imageChangeTimer.alternativeLetter

		PropertyChanges {
			target: pinPukVariantB
			opacity: 1
		}
	}

	Timer {
		id: imageChangeTimer

		property bool alternativeLetter: true

		interval: 6000
		running: passwordType === NumberModel.PASSWORD_PIN || passwordType === NumberModel.PASSWORD_PUK
		repeat: true
		onTriggered: alternativeLetter = !alternativeLetter
	}

	transitions: Transition {
		NumberAnimation {
			properties: "opacity"
			easing.type: Easing.InOutQuad
			duration: Constants.animation_duration
		}
	}

	Image {
		id: pinPukVariantB

		visible: passwordType === NumberModel.PASSWORD_PIN || passwordType === NumberModel.PASSWORD_PUK

		opacity: 0.0
		sourceSize.width: (passwordType === NumberModel.PASSWORD_CAN ? 800 : 560) * ApplicationModel.scaleFactor
		anchors.top: headline.bottom
		anchors.right: parent.right
		anchors.margins: Constants.pane_padding

		verticalAlignment: Image.AlignTop
		fillMode: Image.PreserveAspectFit

		source: "qrc:///images/desktop/pin_letter_pinpuk_different_page.svg"
	}

	NavigationButton {
		id: button

		anchors {
			margins: Constants.component_spacing
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		buttonType: NavigationButton.Type.Back
		onClicked: root.close()
	}
}
