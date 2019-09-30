/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.CardReturnCode 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage
{
	id: baseItem

	signal passwordEntered()
	signal changePinLength()
	signal requestPasswordInfo()

	property alias statusIcon: statusIcon.source
	property int passwordType: NumberModel.passwordType

	Accessible.name: qsTr("Enter %1 view. You can start to enter the number.").arg(d.passwordString) + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the enter password view of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onPressed: {
		if (focus && event.key >= Qt.Key_0 && event.key <= Qt.Key_9) {
			numberField.append(event.key - Qt.Key_0)
			numberField.forceActiveFocus()
			return
		}
	}

	onPasswordTypeChanged: numberField.inputConfirmation = ""
	onVisibleChanged: if (!visible) numberField.text = ""

	QtObject {
		id: d

		readonly property string passwordString: passwordType === NumberModel.PASSWORD_PIN ? "PIN"
											   : passwordType === NumberModel.PASSWORD_NEW_PIN ? "PIN"
											   : passwordType === NumberModel.PASSWORD_CAN ? "CAN"
											   : passwordType === NumberModel.PASSWORD_PUK ? "PUK"
											   : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? "Remote PIN"
											   : "UNKNOWN"

		function setPassword() {
			if (!numberField.validInput) {
				return
			}

			if (passwordType === NumberModel.PASSWORD_PIN) {
				NumberModel.pin = numberField.text
			}
			else if (passwordType === NumberModel.PASSWORD_CAN) {
				NumberModel.can = numberField.text
			}
			else if (passwordType === NumberModel.PASSWORD_PUK) {
				NumberModel.puk = numberField.text
			}
			else if (passwordType === NumberModel.PASSWORD_NEW_PIN) {
				if (numberField.inputConfirmation === "") {
					numberField.inputConfirmation = numberField.text
				} else {
					NumberModel.newPin = numberField.text
					numberField.inputConfirmation = ""
				}
			}
			else if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				RemoteServiceModel.connectToRememberedServer(numberField.text)
			}

			numberField.text = ""
			if (numberField.inputConfirmation === "") {
				baseItem.passwordEntered()
			}
		}
	}

	GText {
		visible: retryCounter.visible
		anchors.horizontalCenter: retryCounter.horizontalCenter
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing

		//: LABEL DESKTOP_QML
		text: qsTr("Attempts") + SettingsModel.translationTrigger
	}

	StatusIcon {
		id: retryCounter

		visible: NumberModel.retryCounter >= 0 && passwordType === NumberModel.PASSWORD_PIN
		height: Style.dimens.status_icon_small
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.margins: height

		activeFocusOnTab: true
		//: LABEL DESKTOP_QML
		Accessible.name: qsTr("Remaining attempts: %1").arg(NumberModel.retryCounter) + SettingsModel.translationTrigger

		text: NumberModel.retryCounter

		FocusFrame {}
	}

	StatusIcon {
		id: statusIcon

		height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: parent.height / 4

		busy: true
		source: AuthModel.readerImage
	}

	GText {
		id: mainText

		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: statusIcon.bottom
		anchors.topMargin: Constants.component_spacing

		activeFocusOnTab: true
		Accessible.role: Accessible.Paragraph
		Accessible.name: mainText.text

		//: LABEL DESKTOP_QML %1 can be "PIN, CAN, PUK or UNKNOWN"
		text: qsTr("%1-Entry").arg(d.passwordString) + SettingsModel.translationTrigger
		textStyle: Style.text.header
		horizontalAlignment: Text.AlignHCenter

		FocusFrame {}
	}

	GText {
		id: subText

		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: mainText.bottom
		anchors.topMargin: Constants.text_spacing

		activeFocusOnTab: true
		Accessible.role: Accessible.Paragraph
		Accessible.name: subText.text
		Keys.onSpacePressed: onLinkActivated(d.passwordString)

		textFormat: Text.StyledText
		linkColor: Constants.white
		text: {
			SettingsModel.translationTrigger

			if (!numberField.confirmedInput) {
				//: INFO DESKTOP_QML The changed PIN was entered wrongfully during the confirmation process.
				return qsTr("The entered PIN does not match the new PIN. Please correct your input.")
			}
			if (passwordType === NumberModel.PASSWORD_PIN) {
				if (NumberModel.requestTransportPin) {
					//: INFO DESKTOP_QML The AA2 expects a PIN with 5 digit which can be entered via the physical or onscreen keyboard.
					return ("%1<br><a href=\"#\">%2</a>").arg(qsTr("Please enter your 5-digit Transport PIN. Use the keyboard or numpad.")).arg(qsTr("More information"))
				} else {
					//: INFO DESKTOP_QML The AA2 expects a PIN with 6 digit which can be entered via the physical or onscreen keyboard.
					return ("%1<br><a href=\"#\">%2</a>").arg(qsTr("Please enter your 6-digit PIN. Use the keyboard or numpad.")).arg(qsTr("More information"))
				}
			}
			if (passwordType === NumberModel.PASSWORD_CAN) {
				if (NumberModel.isCanAllowedMode) {
					//: INFO DESKTOP_QML The user is required to enter the 6 digit CAN.
					return qsTr("Please enter the 6-digit CAN you can find on the front of your ID card.")
				}
				//: INFO DESKTOP_QML The PIN was entered wrongfully twice, this may have happened during an earlier session of the AA2, too. The user needs to enter the CAN for additional verification.
				return ("%1<br><a href=\"#\">%2</a>").arg(qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.")).arg(qsTr("More information"))
			}
			if (passwordType === NumberModel.PASSWORD_PUK) {
				//: INFO DESKTOP_QML The PUK is required to unlock the id card.
				return ("%1<br><a href=\"#\">%2</a>").arg(qsTr("The online identification function is blocked. Please use your personal unblocking key (PUK) to unblock your ID card.")).arg(qsTr("More information"))
			}
			if (passwordType === NumberModel.PASSWORD_NEW_PIN) {
				if (numberField.inputConfirmation === "") {
					//: INFO DESKTOP_QML A new 6-digit PIN needs to be supplied.
					return qsTr("Please enter a new 6-digit PIN of your choice.")
				} else {
					//: INFO DESKTOP_QML The new PIN needs to be entered again for verification.
					return qsTr("Please enter your new 6-digit PIN again.")
				}
			}
			if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				return ("%1<br><a href=\"#\">%2</a>").arg(qsTr("Please start the remote service in order to use your smartphone as a card reader with AusweisApp2.")).arg(qsTr("More information"))
			}

			//: INFO DESKTOP_QML Error message during PIN/CAN/PUK input procedure, the requested password type is unknown; internal error.
			return qsTr("Unknown password type:") + " " + passwordType
		}
		textStyle: numberField.confirmedInput ? Style.text.header_secondary : Style.text.header_warning
		onLinkActivated: baseItem.requestPasswordInfo()

		horizontalAlignment: Text.AlignHCenter

		FocusFrame {}
	}

	Item {
		anchors.top: subText.bottom
		anchors.bottom: button.top
		anchors.horizontalCenter: parent.horizontalCenter

		NumberField {
			id: numberField

			anchors.centerIn: parent

			activeFocusOnTab: true
			Accessible.name: numberField.text

			passwordLength: passwordType === NumberModel.PASSWORD_PIN && NumberModel.requestTransportPin ? 5
						  : passwordType === NumberModel.PASSWORD_PUK ? 10
						  : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? 4
						  : 6

			onVisibleChanged: if (visible) forceActiveFocus()

			MouseArea {
				anchors.fill: parent

				onClicked: numberField.forceActiveFocus()
			}

			Keys.onPressed: {
				if (event.key >= Qt.Key_0 && event.key <= Qt.Key_9) {
					numberField.append(event.key - Qt.Key_0)
					return
				}

				if (event.key === Qt.Key_Backspace) {
					numberField.removeLast()
					return
				}

				if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
					d.setPassword()
				}
			}
		}

		GText {
			id: changePinLengthText

			function changePinLength() {
				baseItem.changePinLength()
			}

			visible: passwordType === NumberModel.PASSWORD_PIN
			anchors.horizontalCenter: numberField.horizontalCenter
			anchors.top: numberField.bottom
			anchors.topMargin: Constants.text_spacing

			activeFocusOnTab: true
			Keys.onSpacePressed: changePinLengthText.changePinLength()
			Accessible.name: text

			textStyle: Style.text.hint
			textFormat: Text.StyledText
			//: LABEL DESKTOP_QML Button, mit dem der Benutzer eine TransportPIN-Änderung starten kann.
			text: "<a href=\"#\">" + (NumberModel.requestTransportPin ? qsTr("Your PIN has 6 digits?") : qsTr("Your PIN has 5 digits?")) + "</a>" + SettingsModel.translationTrigger
			linkColor: Constants.white
			onLinkActivated: changePinLengthText.changePinLength()

			FocusFrame {}
		}
	}

	NumberPad {
		id: numberPad

		anchors.right: parent.right
		anchors.bottom: parent.bottom

		submitEnabled: numberField.validInput
		deleteEnabled: numberField.text.length > 0
		onDigitPressed: numberField.append(digit)
		onDeletePressed: numberField.removeLast()
		onSubmitPressed: d.setPassword()
	}

	NavigationButton {
		id: button

		anchors {
			margins: Constants.component_spacing
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		activeFocusOnTab: true

		buttonType: Qt.ForwardButton
		enabled: numberField.validInput
		onClicked: d.setPassword()
	}
}
