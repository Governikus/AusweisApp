/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PasswordType 1.0


SectionPage {
	id: baseItem

	property alias enableTransportPinLink: transportPinLink.visible
	property int passwordType: NumberModel.passwordType
	property bool isConfirmation: false

	signal passwordEntered(bool pWasNewPin)
	signal changePinLength()

	Keys.onPressed: event => { event.accepted = pinField.handleKeyEvent(event.key, event.modifiers) }

	onVisibleChanged: {
		pinField.number = ""
		if (!isConfirmation) {
			pinField.inputConfirmation = ""
		}
	}

	QtObject {
		id: d

		function setPassword() {
			if (!pinField.validInput) {
				return
			}

			switch(passwordType) {
				case PasswordType.PIN:
				case PasswordType.TRANSPORT_PIN:
				case PasswordType.SMART_PIN:
					NumberModel.pin = pinField.number
					baseItem.passwordEntered(false)
					break
				case PasswordType.NEW_SMART_PIN:
				case PasswordType.NEW_PIN:
					if (isConfirmation) {
						NumberModel.newPin = pinField.number
						baseItem.passwordEntered(true)
					}
					else {
						pinField.inputConfirmation = pinField.number
						pinField.number = ""
					}
					isConfirmation = !isConfirmation
					break
				case PasswordType.CAN:
					NumberModel.can = pinField.number
					baseItem.passwordEntered(false)
					break
				case PasswordType.PUK:
					NumberModel.puk = pinField.number
					baseItem.passwordEntered(false)
					break
				case PasswordType.REMOTE_PIN:
					RemoteServiceModel.connectToRememberedServer(pinField.number)
					baseItem.passwordEntered(false)
					break
			}

			pinField.number = ""
			if (!isConfirmation) {
				pinField.inputConfirmation = ""
			}
		}
	}

	ColumnLayout {
		id: infoLayout

		readonly property real remainingHeight: Math.max(0, infoLayout.height - (infoLayout.children.length - 1) * infoLayout.spacing - infoText.implicitHeight - infoText.Layout.topMargin - infoText.Layout.bottomMargin - mainText.implicitHeight - mainText.Layout.topMargin - mainText.Layout.bottomMargin)

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			bottom: numberPadLayout.top
			margins: Constants.pane_padding
			bottomMargin: 0
		}

		spacing: 0

		StatusIcon {
			id: statusIcon

			readonly property real bottomMargin: Constants.component_spacing

			Layout.fillHeight: true
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: infoLayout.remainingHeight - bottomMargin >= Style.dimens.icon_size ? infoLayout.remainingHeight - bottomMargin : 0
			Layout.maximumHeight: Style.dimens.header_icon_size
			Layout.preferredWidth: Layout.preferredHeight
			Layout.maximumWidth: Layout.maximumHeight
			Layout.bottomMargin: Layout.preferredHeight > 0 ? bottomMargin : 0

			busy: visible
			source: {
				switch(passwordType) {
					case PasswordType.REMOTE_PIN:
						return "qrc:///images/icon_remote_inactive.svg"
					case PasswordType.SMART_PIN:
					case PasswordType.NEW_SMART_PIN:
						return "qrc:///images/mobile/phone_smart.svg"
					default:
						return "qrc:///images/mobile/phone_nfc_with_card.svg"
				}
			}
			borderEnabled: false
		}

		GText {
			id: mainText

			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width

			//: LABEL ANDROID IOS
			text: passwordType === PasswordType.CAN ? qsTr("Enter CAN")
				 //: LABEL ANDROID IOS
				 : passwordType === PasswordType.PUK ? qsTr("Enter PUK")
				 //: LABEL ANDROID IOS
				 : passwordType === PasswordType.REMOTE_PIN ? qsTr("Enter pairing code")
				 //: LABEL ANDROID IOS
				 : (passwordType === PasswordType.NEW_PIN && !isConfirmation)? qsTr("Enter new ID card PIN")
				 //: LABEL ANDROID IOS
				 : (passwordType === PasswordType.NEW_PIN && isConfirmation) ? qsTr("Confirm new ID card PIN")
				 //: LABEL ANDROID IOS
				 : passwordType === PasswordType.TRANSPORT_PIN ? qsTr("Enter Transport PIN")
				 //: LABEL ANDROID IOS
				 : passwordType === PasswordType.SMART_PIN ? qsTr("Enter Smart-eID PIN")
				 //: LABEL ANDROID IOS
				 : (passwordType === PasswordType.NEW_SMART_PIN && !isConfirmation)? qsTr("Enter new Smart-eID PIN")
				 //: LABEL ANDROID IOS
				 : (passwordType === PasswordType.NEW_SMART_PIN && isConfirmation) ? qsTr("Confirm new Smart-eID PIN")
				 //: LABEL ANDROID IOS
				 : qsTr("Enter ID card PIN")
			textStyle: Style.text.header_accent
			horizontalAlignment: Text.AlignHCenter
		}

		GText {
			id: infoText

			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.minimumHeight: textStyle.textSize
			Layout.topMargin: Constants.text_spacing

			elide: Text.ElideRight
			horizontalAlignment: Text.AlignHCenter

			textStyle: {
				if (!pinField.confirmedInput || !!NumberModel.inputError || (passwordType === PasswordType.CAN && !NumberModel.isCanAllowedMode) || passwordType === PasswordType.PUK) {
					return Style.text.normal_warning
				} else {
					return Style.text.normal_secondary
				}
			}
			text: {
				if (!pinField.confirmedInput) {
					return passwordType === PasswordType.NEW_SMART_PIN
						//: INFO ANDROID IOS The changed Smart-eID PIN was entered wrongfully during confirmation.
						? qsTr("The new Smart-eID PIN and the confirmation do not match. Please correct your input.")
						//: INFO ANDROID IOS The changed ID card PIN was entered wrongfully during confirmation.
						: qsTr("The new ID card PIN and the confirmation do not match. Please correct your input.")
				}
				if (!!NumberModel.inputError) {
					return NumberModel.inputError
				}
				if (passwordType === PasswordType.SMART_PIN && NumberModel.retryCounter === 1) {
					//: INFO ANDROID IOS The wrong Smart-eID PIN was entered twice on the Smart-eID
					return qsTr("You have entered an incorrect, six-digit Smart-eID PIN twice. An incorrect third attempt will invalidate your Smart-eID and you will have to set it up again.")
				}
				if (passwordType === PasswordType.CAN) {
					if (NumberModel.isCanAllowedMode) {
						//: INFO ANDROID IOS The user is required to enter the six-digit CAN in CAN-allowed authentication.
						return qsTr("Please enter the six-digit Card Access Number (CAN). You can find it in the bottom right on the front of the ID card.")
					}
					//: INFO ANDROID IOS The wrong ID card PIN was entered twice, the third attempt requires the CAN for additional verification, hint where the CAN is found.
					return qsTr("A wrong ID card PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your CAN in the bottom right on the front of your ID card.")
				}
				if (passwordType === PasswordType.PUK) {
					//: INFO ANDROID IOS The PUK is required to unlock the ID card since the wrong ID card PIN entered three times.
					return qsTr("You have entered an incorrect, six-digit ID card PIN thrice, your ID card PIN is now blocked. To remove the block, the ten-digit PUK must be entered first.")
				}
				if (passwordType === PasswordType.NEW_PIN) {
					return isConfirmation
							//: INFO ANDROID IOS The new ID card PIN needs to be confirmed.
							? qsTr("Please confirm your new six-digit ID card PIN.")
							//: INFO ANDROID IOS A new six-digit ID card PIN needs to be supplied.
							: qsTr("Please enter a new six-digit ID card PIN now.")
				}
				if (passwordType === PasswordType.NEW_SMART_PIN) {
					return isConfirmation
							//: INFO ANDROID IOS The new Smart-eID PIN needs to be confirmed.
							?  qsTr("Please confirm your new six-digit Smart-eID PIN.")
							//: INFO ANDROID IOS A new six-digit Smart-eID PIN needs to be supplied.
							: qsTr("Please enter a new six-digit Smart-eID PIN now.")
				}
				if (passwordType === PasswordType.TRANSPORT_PIN) {
					//: INFO ANDROID IOS The Transport PIN is required by AA2, it needs to be change to an actual PIN.
					return qsTr("Please enter the five-digit Transport PIN.")
				}
				if (passwordType === PasswordType.REMOTE_PIN) {
					//: INFO ANDROID IOS The pairing code for the smartphone is required.
					return qsTr("Enter the pairing code shown on the device you want to pair.")
				}

				if (passwordType === PasswordType.SMART_PIN) {
					return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN
						//: INFO ANDROID IOS The AA2 expects the current Smart-eID PIN with six digits in a PIN change.
						? qsTr("Please enter your current six-digit Smart-eID PIN.")
						//: INFO ANDROID IOS The AA2 expects a Smart-eID PIN with six digits in an authentication.
						: qsTr("Please enter your six-digit Smart-eID PIN.")
				}

				return ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN
					//: INFO ANDROID IOS The AA2 expects the current ID card PIN with six digits in a PIN change.
					? qsTr("Please enter your current six-digit ID card PIN.")
					//: INFO ANDROID IOS The AA2 expects a ID card PIN with six digits in an authentication.
					: qsTr("Please enter your six-digit ID card PIN.")
			}

			MouseArea {
				enabled: infoText.truncated

				anchors.fill: parent
				anchors.margins: -12

				onClicked: completeTextPopup.open()
			}
		}
	}

	ColumnLayout {
		id: numberPadLayout

		anchors {
			left: parent.left
			right: parent.right
			bottom: parent.bottom
			margins: Constants.pane_padding
		}

		spacing: 0

		MoreInformationLink {
			id: transportPinLink

			visible: false
			Layout.fillWidth: true
			Layout.topMargin: Constants.text_spacing

			text: (passwordType === PasswordType.TRANSPORT_PIN ?
				   //: LABEL ANDROID IOS Button to switch to a six-digit ID card PIN.
				   qsTr("Do you have a six-digit ID card PIN?") :
				   //: LABEL ANDROID IOS Button to start a change of the Transport PIN.
				   qsTr("Do you have a five-digit Transport PIN?")
				  )
			horizontalAlignment: Text.AlignHCenter

			onClicked: baseItem.changePinLength()
		}

		Rectangle {
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: pinField.implicitWidth + Constants.component_spacing
			Layout.maximumWidth: Math.min(parent.width, Layout.preferredWidth)
			Layout.preferredHeight: pinField.implicitHeight + Constants.component_spacing
			Layout.topMargin: Constants.component_spacing

			radius: Style.dimens.button_radius
			border.color: Style.color.border
			border.width: Style.dimens.separator_size

			NumberField {
				id: pinField

				anchors {
					fill: parent
					margins: Constants.text_spacing
				}

				onAccepted: d.setPassword()

				passwordLength: passwordType === PasswordType.REMOTE_PIN ? 4
							  : passwordType === PasswordType.TRANSPORT_PIN ? 5
							  : passwordType === PasswordType.PUK ? 10
							  : 6
			}
		}

		NumberPad {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: width
			Layout.preferredHeight: height
			Layout.topMargin: Constants.component_spacing

			submitEnabled: pinField.validInput
			deleteEnabled: pinField.number.length > 0
			onDigitPressed: digit => pinField.append(digit)
			onDeletePressed: {
				pinField.removeLast()
				if (pinField.number.length === 0)
					pinField.forceActiveFocus()
			}
			onSubmitPressed: d.setPassword()
		}
	}

	ConfirmationPopup {
		id: completeTextPopup

		style: ConfirmationPopup.PopupStyle.OkButton
		text: infoText.text
	}
}
