/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage
{
	id: baseItem
	property alias enableTransportPinLink: transportPinLink.enableTransportPinLink
	signal passwordEntered(bool pWasNewPin)
	signal changePinLength()

	onVisibleChanged: {
		pinField.text = ""
		if (state !== "PIN_NEW_AGAIN") {
			pinField.inputConfirmation = ""
		}
	}

	ColumnLayout
	{
		anchors.fill: parent
		anchors.margins: 0

		spacing: 0

		RowLayout {
			Layout.fillWidth: true
			Layout.margins: Constants.pane_padding

			spacing: 0

			Item {/*spacer*/ Layout.fillWidth: true; }

			Image {
				Layout.alignment: Qt.AlignVCenter
				Layout.preferredHeight: 58
				Layout.preferredWidth: 58
				Layout.rightMargin: Constants.component_spacing

				source: baseItem.state === "REMOTE_PIN" ? "qrc:///images/icon_remote.svg" : "qrc:///images/NFCPhoneCard.png"
				fillMode: Image.PreserveAspectFit
			}

			GText {
				Layout.alignment: Qt.AlignVCenter
				Layout.fillWidth: true
				Layout.maximumWidth: Math.ceil(implicitWidth)

				textStyle: {
					if (!pinField.confirmedInput || !!NumberModel.inputError || baseItem.state === "CAN" || baseItem.state === "PUK") {
						Style.text.normal_warning
					} else {
						Style.text.normal_accent
					}
				}
				text: {
					SettingsModel.translationTrigger

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
						return qsTr("Please enter the Transport PIN from your PIN letter.")
					}
					if (baseItem.state === "REMOTE_PIN") {
						//: INFO ANDROID IOS The pairing code for the smartphone is required.
						return qsTr("Enter the pairing code shown on the device you want to pair.")
					}
					return ApplicationModel.currentWorkflow === "changepin"
						   //: LABEL ANDROID IOS
						   ? qsTr("Please enter your current PIN.")
						   //: LABEL ANDROID IOS
						   : qsTr("Please enter your PIN.")
				}
			}

			Item {/*spacer*/ Layout.fillWidth: true; }
		}

		Item {/*spacer*/ Layout.fillHeight: true }

		Rectangle {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredHeight: pinField.height + Constants.component_spacing
			Layout.preferredWidth: pinField.width + Constants.component_spacing

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

				onPasswordLengthChanged: pinField.text = ""
			}
		}

		GText {
			id: transportPinLink

			property alias enableTransportPinLink: myMouse.enabled

			visible: baseItem.state === "PIN" && enableTransportPinLink
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.text_spacing
			Layout.margins: Constants.pane_padding

			Accessible.role: Accessible.Button
			Accessible.onPressAction: if (Qt.platform.os === "ios") myMouse.clicked(null)

			text: (NumberModel.requestTransportPin ?
				   //: LABEL ANDROID IOS Button to switch to a six-digit PIN.
				   qsTr("Does your PIN have six digits?") :
				   //: LABEL ANDROID IOS Button to switch to a Transport PIN or start a change of the Transport PIN.
				   qsTr("Does your PIN have five digits?")
				  ) + SettingsModel.translationTrigger
			textStyle: Style.text.hint_accent
			font.underline: true

			MouseArea {
				id: myMouse

				anchors.fill: parent
				anchors.margins: -12

				enabled: true

				onClicked: baseItem.changePinLength()
			}
		}

		Item {/*spacer*/ Layout.fillHeight: true }

		NumberPad {
			state: baseItem.state

			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: width
			Layout.preferredHeight: height

			submitEnabled: pinField.validInput
			deleteEnabled: pinField.text.length > 0
			onDigitPressed: pinField.append(digit)
			onDeletePressed: pinField.removeLast()
			onSubmitPressed: {
				switch(baseItem.state) {
					case "PIN":
						NumberModel.pin = pinField.text
						baseItem.passwordEntered(false)
						break
					case "PIN_NEW":
						pinField.inputConfirmation = pinField.text
						pinField.text = ""
						baseItem.state = "PIN_NEW_AGAIN"
						break
					case "PIN_NEW_AGAIN":
						NumberModel.newPin = pinField.text
						baseItem.passwordEntered(true)
						break
					case "CAN":
						NumberModel.can = pinField.text
						baseItem.passwordEntered(false)
						break
					case "PUK":
						NumberModel.puk = pinField.text
						baseItem.passwordEntered(false)
						break
					case "REMOTE_PIN":
						RemoteServiceModel.connectToRememberedServer(pinField.text)
						baseItem.passwordEntered(false)
						break
				}
			}
		}
	}
}
