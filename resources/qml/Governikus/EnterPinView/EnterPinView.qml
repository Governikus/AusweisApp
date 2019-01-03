import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.NumberModel 1.0


SectionPage
{
	id: baseItem
	property string remoteDeviceId: ""
	property alias enableTransportPinLink: transportPinLink.enableTransportPinLink
	signal pinEntered()
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
		Item {/*spacer*/ Layout.fillHeight: true; width: parent.width }

		RowLayout {
			spacing: Constants.component_spacing
			width: parent.width

			Item {/*spacer*/ Layout.fillWidth: true; height: parent.height}
			Image {
				Layout.alignment: Qt.AlignVCenter
				width: Utils.dp(58)
				height: width
				// because RowLayout uses implicitHeight that is based on sourceSize we have to explicitly set the sourceSize
				sourceSize.height: baseItem.state === "REMOTE_PIN" ? Utils.dp(58) : 143
				Layout.maximumWidth: width
				source: baseItem.state === "REMOTE_PIN" ? "qrc:///images/icon_remote.svg" : "qrc:///images/NFCPhoneCard.png"
				fillMode: Image.PreserveAspectFit
			}

			Text {
				Layout.alignment: Qt.AlignVCenter
				Layout.fillWidth: true
				Layout.maximumWidth: Math.ceil(implicitWidth)

				wrapMode: Text.WordWrap
				font.pixelSize: text.length > 150 && !Constants.is_tablet? Utils.dp(15) : Constants.header_font_size
				font.bold: true
				color: {
					if (!pinField.confirmedInput || !!NumberModel.inputError || baseItem.state === "CAN" || baseItem.state === "PUK") {
						Constants.red
					} else {
						Constants.blue
					}
				}
				text: {
					settingsModel.translationTrigger

					if (!pinField.confirmedInput) {
						return qsTr("The entered PIN does not match the new PIN. Please correct your input.")
					}
					if (!!NumberModel.inputError) {
						return NumberModel.inputError
					}
					if (baseItem.state === "CAN") {
						if (NumberModel.isCanAllowedMode) {
							return qsTr("Please enter the six-digit card access number. You can find the card access number on the front of the ID card.")
						}
						return qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.")
					}
					if (baseItem.state === "PUK") {
						return qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.")
					}
					if (baseItem.state === "PIN_NEW") {
						return qsTr("Please enter a new 6-digit PIN of your choice.")
					}
					if (baseItem.state === "PIN_NEW_AGAIN") {
						return qsTr("Please enter your new 6-digit PIN again.")
					}
					if (baseItem.state === "PIN" && NumberModel.requestTransportPin) {
						return qsTr("Please enter your transport PIN.")
					}
					if (baseItem.state === "REMOTE_PIN") {
						return qsTr("Enter the pairing code shown on your other device to use it as a card reader.")
					}
					return qsTr("Please enter your personal PIN.")
				}
			}
			Item {/*spacer*/ Layout.fillWidth: true; height: parent.height}
		}

		Item {/*spacer*/ Layout.fillHeight: true; width: parent.width }

		PinField {
			id: pinField
			Layout.alignment: Qt.AlignHCenter
			passwordLength: baseItem.state === "REMOTE_PIN" ? 4
						  : baseItem.state === "PIN" && NumberModel.requestTransportPin ? 5
						  : baseItem.state === "PUK" ? 10
						  : 6
			Layout.preferredHeight: height
			Layout.preferredWidth: width
		}

		Text {
			id: transportPinLink
			property alias enableTransportPinLink: myMouse.enabled

			visible: baseItem.state === "PIN" && enableTransportPinLink
			text: (NumberModel.requestTransportPin ? qsTr("Your PIN has 6 digits?") : qsTr("Your PIN has 5 digits?")) + settingsModel.translationTrigger
			Layout.alignment: Qt.AlignHCenter
			font.pixelSize: Constants.small_font_size
			color: Constants.blue
			font.underline: true

			MouseArea {
				id: myMouse
				enabled: true
				anchors.fill: parent
				anchors.margins: -Utils.dp(12)
				onClicked: baseItem.changePinLength()
			}
		}

		Item {/*spacer*/ height: Constants.component_spacing; width: parent.width }

		PinPad {
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
						baseItem.pinEntered()
						break
					case "PIN_NEW":
						pinField.inputConfirmation = pinField.text
						pinField.text = ""
						baseItem.state = "PIN_NEW_AGAIN"
						break
					case "PIN_NEW_AGAIN":
						NumberModel.newPin = pinField.text
						baseItem.pinEntered()
						break
					case "CAN":
						NumberModel.can = pinField.text
						baseItem.pinEntered()
						break
					case "PUK":
						NumberModel.puk = pinField.text
						baseItem.pinEntered()
						break
					case "REMOTE_PIN":
						RemoteServiceModel.connectToServer(remoteDeviceId, pinField.text)
						baseItem.pinEntered()
						break
				}
			}
		}

		Item {/*spacer*/ Layout.fillHeight: true; width: parent.width }
	}
}
