/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


SectionPage
{
	id: baseItem

	signal passwordEntered(bool pWasNewPin)
	signal requestPasswordInfo()
	signal changePinLength()

	property alias enableTransportPinLink: transportPinLink.visible
	property alias statusIcon: statusIcon.source
	property int passwordType: NumberModel.passwordType
	property bool requestTransportPin: NumberModel.requestTransportPin

	//: LABEL DESKTOP_QML %1 is the title, e.g. "PIN entry"
	Accessible.name: qsTr("%1. You can start to enter the number.").arg(mainText.text)
	Accessible.description: qsTr("This is the enter password view of the AusweisApp2.")
	Keys.onPressed: event.accepted = numberField.handleKeyEvent(event.key, event.modifiers)

	onPasswordTypeChanged: numberField.inputConfirmation = ""
	onVisibleChanged: if (!visible) numberField.number = ""

	QtObject {
		id: d

		function setPassword() {
			if (!numberField.validInput) {
				return
			}

			let wasNewPin = false;
			if (passwordType === NumberModel.PASSWORD_PIN) {
				NumberModel.pin = numberField.number
			}
			else if (passwordType === NumberModel.PASSWORD_CAN) {
				NumberModel.can = numberField.number
			}
			else if (passwordType === NumberModel.PASSWORD_PUK) {
				NumberModel.puk = numberField.number
			}
			else if (passwordType === NumberModel.PASSWORD_NEW_PIN) {
				if (numberField.inputConfirmation === "") {
					numberField.inputConfirmation = numberField.number
				} else {
					NumberModel.newPin = numberField.number
					numberField.inputConfirmation = ""
					wasNewPin = true
				}
			}
			else if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				RemoteServiceModel.connectToRememberedServer(numberField.number)
			}

			numberField.number = ""
			if (numberField.inputConfirmation === "") {
				baseItem.passwordEntered(wasNewPin)
			}
		}
	}

	GText {
		visible: retryCounter.visible
		anchors.horizontalCenter: retryCounter.horizontalCenter
		anchors.bottom: retryCounter.top
		anchors.bottomMargin: Constants.component_spacing

		//: LABEL DESKTOP_QML
		text: qsTr("Attempts")
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
		Accessible.name: qsTr("Remaining attempts: %1").arg(NumberModel.retryCounter)

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
		text: passwordType === NumberModel.PASSWORD_CAN ? qsTr("Enter CAN")
			 //: LABEL DESKTOP_QML
			 : passwordType === NumberModel.PASSWORD_PUK ? qsTr("Enter PUK")
			 //: LABEL DESKTOP_QML
			 : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? qsTr("Enter pairing code")
			 //: LABEL DESKTOP_QML
			 : passwordType === NumberModel.PASSWORD_NEW_PIN && numberField.inputConfirmation === "" ? qsTr("Enter new PIN")
			 //: LABEL DESKTOP_QML
			 : passwordType === NumberModel.PASSWORD_NEW_PIN ? qsTr("Confirm new PIN")
			 //: LABEL DESKTOP_QML
			 : baseItem.requestTransportPin ? qsTr("Enter Transport PIN")
			 //: LABEL DESKTOP_QML
			 : qsTr("Enter PIN")
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
			if (!numberField.confirmedInput) {
				//: INFO DESKTOP_QML The changed PIN was entered wrongfully during the confirmation process.
				return qsTr("The new PIN and the confirmation do not match. Please correct your input.")
			}
			if (passwordType === NumberModel.PASSWORD_PIN) {
				if (baseItem.requestTransportPin) {
					return ("%1<br><br><a href=\"#\">%2</a>").arg(
							   //: INFO DESKTOP_QML The AA2 expects the Transport PIN with five digits.
							   qsTr("Please enter the five-digit Transport PIN.")
						   ).arg(
							   //: INFO DESKTOP_QML Link text
							   qsTr("More information")
						   )
				} else {
					return ("%1<br><br><a href=\"#\">%2</a>").arg(
							   ApplicationModel.currentWorkflow === "changepin"
							   //: INFO DESKTOP_QML The AA2 expects the current PIN with six digits in a PIN change.
							   ? qsTr("Please enter your current six-digit PIN.")
							   //: INFO DESKTOP_QML The AA2 expects a PIN with six digits in an authentication.
							   : qsTr("Please enter your six-digit PIN.")
						   ).arg(
							   //: INFO DESKTOP_QML Link text
							   qsTr("More information")
						   )
				}
			}
			if (passwordType === NumberModel.PASSWORD_CAN) {
				return ("%1<br><br><a href=\"#\">%2</a>").arg(
							//: INFO DESKTOP_QML The user is required to enter the six-digit CAN in CAN-allowed authentication.
							qsTr("Please enter the six-digit Card Access Number (CAN). You can find your Card Access Number (CAN) in the bottom right on the front of the ID card.")
						).arg(
							//: INFO DESKTOP_QML Link text
							qsTr("More information")
						)
			}
			if (passwordType === NumberModel.PASSWORD_PUK) {
				return ("%1<br><br><a href=\"#\">%2</a>").arg(
						   //: INFO DESKTOP_QML The PUK is required to unlock the ID card.
						   qsTr("The PIN of your ID card is blocked after three incorrect attempts. Please enter the Personal Unblocking Key (PUK) to lift the block. You can find the key in your PIN letter.")
					   ).arg(
						   //: INFO DESKTOP_QML Link text
						   qsTr("More information")
					   )
			}
			if (passwordType === NumberModel.PASSWORD_NEW_PIN) {
				if (numberField.inputConfirmation === "") {
					//: INFO DESKTOP_QML A new six-digit PIN needs to be supplied.
					return qsTr("Please enter a new six-digit PIN now.")
				} else {
					//: INFO DESKTOP_QML The new PIN needs to be entered again for verification.
					return qsTr("Please confirm your new six-digit PIN.")
				}
			}
			if (passwordType === NumberModel.PASSWORD_REMOTE_PIN) {
				return ("%1<br><br><a href=\"#\">%2</a>").arg(
						   //: INFO DESKTOP_QML The pairing code needs to be supplied.
						   qsTr("Start the pairing on your smartphone and enter the pairing code shown there in order to use your smartphone as a card reader (SaC).")
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

			passwordLength: passwordType === NumberModel.PASSWORD_PIN && baseItem.requestTransportPin ? 5
						  : passwordType === NumberModel.PASSWORD_PUK ? 10
						  : passwordType === NumberModel.PASSWORD_REMOTE_PIN ? 4
						  : 6

			onVisibleChanged: if (visible) forceActiveFocus()

			Keys.onPressed: {
				if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
					d.setPassword()
				}
			}
		}

		GText {
			id: transportPinLink

			visible: false
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
			//: LABEL DESKTOP_QML Button to switch to start a change of the Transport PIN.
			text: "<a href=\"#\">%1</a>".arg(qsTr("Do you have a five-digit Transport PIN?"))
			linkColor: textStyle.textColor
			onLinkActivated: baseItem.changePinLength()

			FocusFrame {}
		}
	}

	NumberPad {
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		submitEnabled: numberField.validInput
		deleteEnabled: numberField.number.length > 0
		onDigitPressed: numberField.append(digit)
		onDeletePressed: {
			numberField.removeLast()
			if (numberField.number.length === 0)
				numberField.forceActiveFocus()
		}
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
