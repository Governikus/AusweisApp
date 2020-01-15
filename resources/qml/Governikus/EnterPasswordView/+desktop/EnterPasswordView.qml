/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
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

	//: LABEL DESKTOP_QML %1 is the title, e.g. "PIN entry"
	Accessible.name: qsTr("%1. You can start to enter the number.").arg(mainText.text) + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the enter password view of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onPressed: {
		if (focus && event.key >= Qt.Key_0 && event.key <= Qt.Key_9) {
			numberField.append(event.key - Qt.Key_0)
			numberField.forceActiveFocus()
		}
	}

	onPasswordTypeChanged: numberField.inputConfirmation = ""
	onVisibleChanged: if (!visible) numberField.text = ""

	QtObject {
		id: d

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
		Accessible.role: Accessible.StaticText
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
		Accessible.name: mainText.text

		//: LABEL DESKTOP_QML
		text: (passwordType === NumberModel.PASSWORD_PIN ? qsTr("PIN entry")
			   //: LABEL DESKTOP_QML
		     : passwordType === NumberModel.PASSWORD_NEW_PIN ? qsTr("PIN entry")
			   //: LABEL DESKTOP_QML
		     : passwordType === NumberModel.PASSWORD_CAN ? qsTr("CAN entry")
			   //: LABEL DESKTOP_QML
		     : passwordType === NumberModel.PASSWORD_PUK ? qsTr("PUK entry")
			   //: LABEL DESKTOP_QML
		     : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? qsTr("Pairing code")
		     : console.error("Unknown type")) + SettingsModel.translationTrigger
		textStyle: Style.text.header_inverse
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
		Accessible.name: subText.text

		textFormat: Text.StyledText
		linkColor: Style.text.header_inverse.textColor
		text: {
			SettingsModel.translationTrigger

			if (!numberField.confirmedInput) {
				//: INFO DESKTOP_QML The changed PIN was entered wrongfully during the confirmation process.
				return qsTr("The new PIN and the confirmation do not match. Please correct your input.")
			}
			if (passwordType === NumberModel.PASSWORD_PIN) {
				if (NumberModel.requestTransportPin) {
					return ("%1<br><a href=\"#\">%2</a>").arg(
							   //: INFO DESKTOP_QML The AA2 expects a PIN with 5 digits which can be entered via the physical or onscreen keyboard
							   qsTr("Please enter your 5-digit Transport PIN. Use the keyboard or numpad.")
						   ).arg(
							   //: INFO DESKTOP_QML Link text
							   qsTr("More information")
						   )
				} else {
					return ("%1<br><a href=\"#\">%2</a>").arg(
							   //: INFO DESKTOP_QML The AA2 expects a PIN with 6 digits which can be entered via the physical or onscreen keyboard.
							   qsTr("Please enter your 6-digit PIN. Use the keyboard or numpad.")
						   ).arg(
							   //: INFO DESKTOP_QML Link text
							   qsTr("More information")
						   )
				}
			}
			if (passwordType === NumberModel.PASSWORD_CAN) {
				if (NumberModel.isCanAllowedMode) {
					return ("%1<br><a href=\"#\">%2</a>").arg(
							   //: INFO DESKTOP_QML The operator is required to enter the 6-digit CAN in CAN-allowed authentication.
							   qsTr("Please enter the 6-digit card access number (CAN).")
						   ).arg(
							   //: INFO DESKTOP_QML Link text
							   qsTr("More information")
						   )
				}
				return ("%1<br><a href=\"#\">%2</a>").arg(
						   //: INFO DESKTOP_QML The PIN was entered wrongfully twice, this may have happened during an earlier session of the AA2, too. The user needs to enter the CAN for additional verification.
						   qsTr("A wrong PIN has been entered twice on your ID card. Prior to a third attempt, you have to enter your 6-digit card access number (CAN) first. You can find your card access number (CAN) on the front of your ID card.")
					   ).arg(
						   //: INFO DESKTOP_QML Link text
						   qsTr("More information")
					   )
			}
			if (passwordType === NumberModel.PASSWORD_PUK) {
				return ("%1<br><a href=\"#\">%2</a>").arg(
						   //: INFO DESKTOP_QML The PUK is required to unlock the ID card.
						   qsTr("The PIN of your ID card is blocked after three incorrect tries. The block can be lifted by entering your personal unblocking key (PUK).")
					   ).arg(
						   //: INFO DESKTOP_QML Link text
						   qsTr("More information")
					   )
			}
			if (passwordType === NumberModel.PASSWORD_NEW_PIN) {
				if (numberField.inputConfirmation === "") {
					//: INFO DESKTOP_QML A new 6-digit PIN needs to be supplied.
					return qsTr("Please enter a new 6-digit PIN now.")
				} else {
					//: INFO DESKTOP_QML The new PIN needs to be entered again for verification.
					return qsTr("Please confirm your new 6-digit PIN.")
				}
			}
			if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				return ("%1<br><a href=\"#\">%2</a>").arg(
						   //: INFO DESKTOP_QML The pairing code needs to be supplied.
						   qsTr("Start the pairing on your smartphone and enter the shown pairing code in order to use your smartphone as a card reader (SaC).")
					   ).arg(
						   //: INFO DESKTOP_QML Link text
						   qsTr("More information")
					   )
			}

			//: INFO DESKTOP_QML Error message during PIN/CAN/PUK input procedure, the requested password type is unknown; internal error.
			return qsTr("Unknown password type:") + " " + passwordType
		}
		textStyle: numberField.confirmedInput ? Style.text.header_secondary_inverse : Style.text.header_warning
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

			anchors {
				centerIn: parent
				horizontalCenterOffset: eyeWidth / 2
			}

			passwordLength: passwordType === NumberModel.PASSWORD_PIN && NumberModel.requestTransportPin ? 5
						  : passwordType === NumberModel.PASSWORD_PUK ? 10
						  : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? 4
						  : 6

			onPasswordLengthChanged: numberField.text = ""
			onVisibleChanged: if (visible) forceActiveFocus()

			Keys.onPressed: {
				if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
					d.setPassword()
				}
			}
		}

		GText {
			visible: passwordType === NumberModel.PASSWORD_PIN
			anchors {
				horizontalCenter: parent.horizontalCenter
				top: numberField.bottom
				topMargin: Constants.component_spacing
			}

			activeFocusOnTab: true
			Accessible.name: text
			Accessible.role: Accessible.Button

			textStyle: Style.text.hint_inverse
			textFormat: Text.StyledText
			text: "<a href=\"#\">" + (NumberModel.requestTransportPin ?
									  //: LABEL DESKTOP_QML Button to switch to a 6-digit PIN.
									  qsTr("Does your PIN have 6 digits?") :
									  //: LABEL DESKTOP_QML Button to switch to a transport PIN or start a change of the transport PIN.
									  qsTr("Does your PIN have 5 digits?")
									 ) + "</a>" + SettingsModel.translationTrigger
			linkColor: textStyle.textColor
			onLinkActivated: baseItem.changePinLength()

			FocusFrame {}
		}
	}

	NumberPad {
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

		buttonType: NavigationButton.Type.Forward
		enabled: numberField.validInput
		onClicked: {
			d.setPassword()
			numberField.focus = true
		}
	}
}
