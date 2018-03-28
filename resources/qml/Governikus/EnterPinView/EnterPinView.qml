import QtQuick 2.5
import QtQuick.Layouts 1.1

import Governikus.Global 1.0

SectionPage
{
	id: baseItem
	property string remoteDeviceId: ""
	signal pinEntered()

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
				anchors.verticalCenter: parent.verticalCenter
				width: Utils.dp(58)
				height: width
				// because RowLayout uses implicitHeight that is based on sourceSize we have to explicitly set the sourceSize
				sourceSize.height: baseItem.state === "REMOTE_PIN" ? Utils.dp(58) : 143
				Layout.maximumWidth: width
				source: baseItem.state === "REMOTE_PIN" ? "qrc:///images/icon_remote.svg" : "qrc:///images/NFCPhoneCard.png"
				fillMode: Image.PreserveAspectFit
			}

			Text {
				anchors.verticalCenter: parent.verticalCenter
				Layout.fillWidth: true
				Layout.maximumWidth: Math.ceil(implicitWidth)

				wrapMode: Text.WordWrap
				font.pixelSize: text.length > 150 && !PlatformConstants.is_tablet? Utils.dp(15) : Constants.header_font_size
				font.bold: true
				color: {
					if (!pinField.confirmedInput || !!numberModel.inputError || baseItem.state === "CAN" || baseItem.state === "PUK") {
						Constants.red
					} else {
						Constants.blue
					}
				}
				text: (!pinField.confirmedInput ? qsTr("The entered PIN does not match the new PIN. Please correct your input.") :
					  !!numberModel.inputError ? numberModel.inputError :
					  baseItem.state === "CAN" ? qsTr("You have entered the wrong PIN twice. Prior to a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.") :
					  baseItem.state === "PUK" ? qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.") :
					  baseItem.state === "PIN_NEW" ? qsTr("Please enter a new 6-digit PIN of your choice.") :
					  baseItem.state === "PIN_NEW_AGAIN" ? qsTr("Please enter your new 6-digit PIN again.") :
					  baseItem.state === "PIN" ? qsTr("Please enter your personal PIN.") :
					  baseItem.state === "REMOTE_PIN" ? qsTr("Enter the pairing code shown on your other device to use it as a card reader.") :
					  /*"PIN_OR_TRANSPORT_PIN"*/ qsTr("Please enter your current PIN or your initial transport PIN first.")
					  ) + settingsModel.translationTrigger
			}
			Item {/*spacer*/ Layout.fillWidth: true; height: parent.height}
		}

		Item {/*spacer*/ Layout.fillHeight: true; width: parent.width }

		PinField {
			id: pinField
			anchors.horizontalCenter: parent.horizontalCenter
			state: baseItem.state
			Layout.preferredWidth: width
		}

		Item {/*spacer*/ height: Constants.component_spacing; width: parent.width }

		PinPad {
			anchors.horizontalCenter: parent.horizontalCenter
			state: baseItem.state
			Layout.preferredWidth: width
			Layout.preferredHeight: height

			submitEnabled: pinField.validInput
			deleteEnabled: pinField.text.length > 0
			onDigitPressed: pinField.append(digit)
			onDeletePressed: pinField.removeLast()
			onSubmitPressed: {
				switch(baseItem.state) {
					case "PIN":
						/* fall through */
					case "PIN_OR_TRANSPORT_PIN":
						numberModel.pin = pinField.text
						baseItem.pinEntered()
						break
					case "PIN_NEW":
						pinField.inputConfirmation = pinField.text
						pinField.text = ""
						baseItem.state = "PIN_NEW_AGAIN"
						break
					case "PIN_NEW_AGAIN":
						numberModel.newPin = pinField.text
						baseItem.pinEntered()
						break
					case "CAN":
						numberModel.can = pinField.text
						baseItem.pinEntered()
						break
					case "PUK":
						numberModel.puk = pinField.text
						baseItem.pinEntered()
						break
					case "REMOTE_PIN":
						remoteServiceModel.connectToServer(remoteDeviceId, pinField.text)
						baseItem.pinEntered()
						break
				}
			}
		}

		Item {/*spacer*/ Layout.fillHeight: true; width: parent.width }
	}

}
