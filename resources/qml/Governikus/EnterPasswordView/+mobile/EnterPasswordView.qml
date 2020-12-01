/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage {
	id: baseItem

	property alias enableTransportPinLink: transportPinLink.enableTransportPinLink
	signal passwordEntered(bool pWasNewPin)
	signal changePinLength()

	onVisibleChanged: {
		pinField.number = ""
		if (state !== "PIN_NEW_AGAIN") {
			pinField.inputConfirmation = ""
		}
	}

	Item {
		id: container

		height: parent.height // Avoid binding loop when no parent is set e.g. while pushing it to a StackView as `Component { EnterPasswordView {} }`.
		anchors {
			fill: parent
			margins: Constants.pane_padding
		}

		readonly property real minIconHeight: Style.dimens.icon_size
		readonly property real iconHeight: Math.min(container.height - layout.implicitHeight, Style.dimens.header_icon_size)
		readonly property real elementHeight: layout.implicitHeight + minIconHeight
		readonly property bool elementsFitOnScreen: elementHeight <= container.height

		StatusIcon {
			id: statusIcon

			visible: container.elementsFitOnScreen
			height: visible ? container.iconHeight : 0
			width: height
			anchors {
				top: parent.top
				horizontalCenter: parent.horizontalCenter
			}

			busy: visible
			source: baseItem.state === "REMOTE_PIN" ? "qrc:///images/icon_remote_inactive.svg" : "qrc:///images/mobile/phone_nfc_with_card.svg"
			borderEnabled: false
		}

		ColumnLayout {
			id: layout

			anchors {
				left: parent.left
				right: parent.right
				top: statusIcon.bottom
				bottom: parent.bottom
			}

			spacing: 0

			GText {
				id: mainText

				Layout.fillWidth: true
				Layout.alignment: Qt.AlignHCenter
				Layout.maximumWidth: Style.dimens.max_text_width
				Layout.topMargin: Constants.component_spacing

				//: LABEL ANDROID IOS
				text: baseItem.state === "CAN" ? qsTr("Enter CAN")
					 //: LABEL ANDROID IOS
					 : baseItem.state === "PUK" ? qsTr("Enter PUK")
					 //: LABEL ANDROID IOS
					 : baseItem.state === "REMOTE_PIN" ? qsTr("Enter pairing code")
					 //: LABEL ANDROID IOS
					 : baseItem.state === "PIN_NEW" ? qsTr("Enter new PIN")
					 //: LABEL ANDROID IOS
					 : baseItem.state === "PIN_NEW_AGAIN" ? qsTr("Confirm new PIN")
					 //: LABEL ANDROID IOS
					 : baseItem.state === "PIN" && NumberModel.requestTransportPin ? qsTr("Enter Transport PIN")
					 //: LABEL ANDROID IOS
					 : qsTr("Enter PIN")
				textStyle: Style.text.header_accent
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				id: infoText

				Layout.fillWidth: true
				Layout.fillHeight: true
				Layout.alignment: Qt.AlignHCenter
				Layout.maximumWidth: Style.dimens.max_text_width
				Layout.topMargin: Constants.text_spacing

				elide: Text.ElideRight
				horizontalAlignment: Text.AlignHCenter

				textStyle: {
					if (!pinField.confirmedInput || !!NumberModel.inputError || baseItem.state === "CAN" || baseItem.state === "PUK") {
						return Style.text.normal_warning
					} else {
						return Style.text.normal_secondary
					}
				}
				text: {
					if (!pinField.confirmedInput) {
						//: INFO ANDROID IOS The changed PIN was entered wrongfully during confirmation.
						return qsTr("The new PIN and the confirmation do not match. Please correct your input.")
					}
					if (!!NumberModel.inputError) {
						return NumberModel.inputError
					}
					if (baseItem.state === "CAN") {
						if (NumberModel.isCanAllowedMode) {
							//: INFO ANDROID IOS The CAN needs to be entered in CAN-allowed mode, hint where the CAN can be found.
							return qsTr("Please enter the six-digit Card Access Number (CAN). You can find it in the bottom right on the front of the ID card.")
						}
						//: INFO ANDROID IOS The wrong PIN was entered twice, the third attempt requires the CAN for additional verification, hint where the CAN is found.
						return qsTr("A wrong PIN has been entered twice on your ID card. For a third attempt, please first enter the six-digit Card Access Number (CAN). You can find your Card Access Number (CAN) in the bottom right on the front of your ID card.")
					}
					if (baseItem.state === "PUK") {
						//: INFO ANDROID IOS The PUK is required to unlock the ID card since the wrong PIN entered three times.
						return qsTr("A wrong PIN has been entered three times on your ID card. Your PIN is now blocked. To unblock your PIN you have to enter the PUK.")
					}
					if (baseItem.state === "PIN_NEW") {
						//: INFO ANDROID IOS A new six-digit PIN needs to be supplied.
						return qsTr("Please enter a new six-digit PIN now.")
					}
					if (baseItem.state === "PIN_NEW_AGAIN") {
						//: INFO ANDROID IOS The new PIN needs to be confirmed.
						return qsTr("Please confirm your new six-digit PIN.")
					}
					if (baseItem.state === "PIN" && NumberModel.requestTransportPin) {
						//: INFO ANDROID IOS The Transport PIN is required by AA2, it needs to be change to an actual PIN.
						return qsTr("Please enter the five-digit Transport PIN.")
					}
					if (baseItem.state === "REMOTE_PIN") {
						//: INFO ANDROID IOS The pairing code for the smartphone is required.
						return qsTr("Enter the pairing code shown on the device you want to pair.")
					}

					return ApplicationModel.currentWorkflow === "changepin"
						//: INFO ANDROID IOS The AA2 expects the current PIN with six digits in a PIN change.
						? qsTr("Please enter your current six-digit PIN.")
						//: INFO ANDROID IOS The AA2 expects a PIN with six digits in an authentication.
						: qsTr("Please enter your six-digit PIN.")
				}

				MouseArea {
					enabled: infoText.truncated

					anchors.fill: parent
					anchors.margins: -12

					onClicked: completeTextPopup.open()
				}
			}

			GText {
				id: transportPinLink

				property bool enableTransportPinLink: false

				visible: baseItem.state === "PIN" && enableTransportPinLink
				Layout.fillWidth: true
				Layout.topMargin: Constants.text_spacing

				Accessible.role: Accessible.Button
				Accessible.onPressAction: if (Qt.platform.os === "ios") myMouse.clicked(null)

				text: (NumberModel.requestTransportPin ?
					   //: LABEL ANDROID IOS Button to switch to a six-digit PIN.
					   qsTr("Do you have a six-digit PIN?") :
					   //: LABEL ANDROID IOS Button to start a change of the Transport PIN.
					   qsTr("Do you have a five-digit Transport PIN?")
					  )
				textStyle: Style.text.normal_accent
				font.underline: true
				horizontalAlignment: Text.AlignHCenter

				MouseArea {
					id: myMouse

					anchors.fill: parent
					anchors.margins: -12

					enabled: transportPinLink.enableTransportPinLink

					onClicked: baseItem.changePinLength()
				}
			}

			Rectangle {
				Layout.alignment: Qt.AlignHCenter
				Layout.preferredHeight: pinField.height + Constants.component_spacing
				Layout.preferredWidth: pinField.width + Constants.component_spacing
				Layout.topMargin: Constants.component_spacing

				radius: Style.dimens.button_radius
				border.color: Style.color.border
				border.width: Style.dimens.separator_size

				NumberField {
					id: pinField

					anchors.centerIn: parent

					passwordLength: baseItem.state === "REMOTE_PIN" ? 4
								  : baseItem.state === "PIN" && NumberModel.requestTransportPin ? 5
								  : baseItem.state === "PUK" ? 10
								  : 6
				}
			}

			NumberPad {
				state: baseItem.state

				Layout.alignment: Qt.AlignHCenter
				Layout.preferredWidth: width
				Layout.preferredHeight: height
				Layout.topMargin: Constants.component_spacing

				submitEnabled: pinField.validInput
				deleteEnabled: pinField.number.length > 0
				onDigitPressed: pinField.append(digit)
				onDeletePressed: {
					pinField.removeLast()
					if (pinField.number.length === 0)
						pinField.forceActiveFocus()
				}
				onSubmitPressed: {
					switch(baseItem.state) {
						case "PIN":
							NumberModel.pin = pinField.number
							baseItem.passwordEntered(false)
							break
						case "PIN_NEW":
							pinField.inputConfirmation = pinField.number
							pinField.number = ""
							baseItem.state = "PIN_NEW_AGAIN"
							break
						case "PIN_NEW_AGAIN":
							NumberModel.newPin = pinField.number
							baseItem.passwordEntered(true)
							break
						case "CAN":
							NumberModel.can = pinField.number
							baseItem.passwordEntered(false)
							break
						case "PUK":
							NumberModel.puk = pinField.number
							baseItem.passwordEntered(false)
							break
						case "REMOTE_PIN":
							RemoteServiceModel.connectToRememberedServer(pinField.number)
							baseItem.passwordEntered(false)
							break
					}
				}
			}
		}
	}

	ConfirmationPopup {
		id: completeTextPopup

		style: ConfirmationPopup.PopupStyle.OkButton
		text: infoText.text
	}
}
