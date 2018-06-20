import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: rootPage
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Configure remote service") + settingsModel.translationTrigger; font.bold: true }

	Connections {
		target: remoteServiceModel
		onFirePairingFailed: qmlExtension.showFeedback(qsTr("Pairing failed. Please try again to activate pairing on your other device and enter the shown pairing code."))
	}

	content: Column {
		id: mainColumn
		width: rootPage.width
		padding: Constants.pane_padding
		spacing: Constants.component_spacing

		readonly property int usableWidth: width - 2 * padding

		Text {
			id: errorMsg
			width: parent.usableWidth
			text: ""
			color: Constants.red
			font.pixelSize: Constants.normal_font_size
			wrapMode: Text.WordWrap
			verticalAlignment: Text.AlignVCenter
			visible: text !== ""
		}

		Column {
			id: nameContainer
			width: parent.usableWidth

			Text {
				text: qsTr("Device name") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.blue
			}

			Text {
				color: Constants.secondary_text
				text: qsTr("Set device name:") + settingsModel.translationTrigger
				width: parent.width
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			Item {
				id: spacing
				height: Utils.dp(5)
				width: height
			}

			GTextField {
				id: serverName123
				width: parent.width
				onAccepted: {
					settingsModel.serverName = text
					text = settingsModel.serverName
				}
				onVisibleChanged: if (visible) text = settingsModel.serverName
			}
		}

		Item {
			id: pinPadModeContainer
			visible: Qt.platform.os !== "ios"
			width: parent.usableWidth
			height: Math.max(pinPadModeText.height, pinPadModeSwitch.height)

			Item {
				id: pinPadModeText
				height: nameText.height + dateText.height
				anchors.left: pinPadModeContainer.left
				anchors.right: pinPadModeSwitch.left
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: pinPadModeContainer.verticalCenter
				Text {
					id: nameText
					anchors.bottomMargin: Utils.dp(2)
					font.pixelSize: Constants.normal_font_size
					color: Constants.blue
					font.bold: true
					text: qsTr("PIN pad mode") + settingsModel.translationTrigger
				}

				Text {
					id: dateText
					width: parent.width
					anchors.top: nameText.bottom
					font.pixelSize: Constants.normal_font_size
					color: Constants.secondary_text
					text: qsTr("Enter PIN on this device") + settingsModel.translationTrigger
					wrapMode: Text.WordWrap
				}
			}

			GSwitch {
				id: pinPadModeSwitch
				anchors.right: pinPadModeContainer.right
				anchors.verticalCenter: pinPadModeContainer.verticalCenter
				onIsOnChanged: settingsModel.pinPadMode = pinPadModeSwitch.isOn
				onVisibleChanged: if (visible) isOn = settingsModel.pinPadMode
			}
		}

		Column {
			width: parent.usableWidth

			Text {
				text: qsTr("Paired devices") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.blue
			}

			Text {
				color: Constants.secondary_text
				text: qsTr("No device is paired.") + settingsModel.translationTrigger
				width: parent.width
				visible: !knownDeviceList.visible
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			ListView {
				id: knownDeviceList
				width: parent.width
				height: childrenRect.height
				model: remoteServiceModel.knownDevices
				delegate: KnownDevicesListDelegate {
					width: knownDeviceList.width
				}
				visible: count > 0
				interactive: false
			}
		}

		Column {
			width: parent.usableWidth

			Text {
				text: qsTr("Available devices") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.blue
			}

			Text {
				color: Constants.secondary_text
				text: qsTr("No new remote reader was found on your network. Make sure that the remote reader functionality in AusweisApp2 on your other device is activated and that your devices are connected to the same network.") + settingsModel.translationTrigger
				width: parent.width
				visible: !searchDeviceList.visible
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			ListView {
				id: searchDeviceList
				width: parent.width
				height: childrenRect.height
				model: remoteServiceModel.availableRemoteDevices
				delegate: AvailableDevicesListDelegate {
					width: searchDeviceList.width
					onRequestPairing: {
						enterPinView.remoteDeviceId = pDeviceId
						informationPairingPopup.open()
					}
				}
				visible: count > 0
				interactive: false
			}
		}
	}

	Popup {
		id: informationPairingPopup
		x: (rootPage.width - width) / 2
		y: (rootPage.height - height) / 2
		width: Utils.dp(250)
		height: contentColumn.height + 2 * Constants.pane_padding
		modal: true
		focus: true
		padding: Constants.pane_padding
		closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

		Column {
			id: contentColumn
			width: parent.width
			spacing: Constants.pane_spacing

			Text {
				id: info
				color: Constants.secondary_text
				width: parent.width
				wrapMode: Text.WordWrap
				font.pixelSize: Constants.normal_font_size
				text: qsTr("Please start pairing mode first.") + settingsModel.translationTrigger
			}

			GButton {
				text: qsTr("OK") + settingsModel.translationTrigger
				width: parent.width

				onClicked: {
					informationPairingPopup.close()
					firePush(enterPinView, {})
				}
			}
		}
	}

	EnterPinView {
		id: enterPinView
		state: "REMOTE_PIN"
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: firePop() }
		headerTitleBarAction: TitleBarAction { text: qsTr("Pairing code") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			firePop()
		}
	}
}
