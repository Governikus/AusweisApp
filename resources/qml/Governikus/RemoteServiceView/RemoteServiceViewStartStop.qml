import QtQuick 2.10

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.TitleBar 1.0

Item {
	id: baseItem

	readonly property int maxWidth: Math.min(width - 2 * Constants.component_spacing, Utils.dp(500))

	RemoteServicePairingPopup {
		id: popup
		x: (baseItem.width - width) / 2
		y: (baseItem.height - height) / 2

		pin: RemoteServiceModel.psk.toString()
		onPinChanged: {
			if (pin === "") {
				close()
			}
		}
		onVisibleChanged: RemoteServiceModel.setPairing(visible)
	}

	Image {
		id: image
		source: "qrc:///images/phone_to_pc.svg"
		anchors.top: parent.top
		anchors.margins: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter
		height: parent.height * 0.2
		width: parent.maxWidth
		fillMode: Image.PreserveAspectFit
	}

	Text {
		id: text
		color: Constants.secondary_text

		width: parent.maxWidth
		anchors.top: image.bottom
		anchors.margins: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		text: qsTr("Please start the remote service in order to use your smartphone as a card reader with AusweisApp2."
				+ " Please note: Both your devices have to be connected to the same WiFi.")
				+ settingsModel.translationTrigger
		font.pixelSize: Constants.normal_font_size
		wrapMode: Text.WordWrap
	}


	GButton {
		readonly property bool running: RemoteServiceModel.running
		readonly property bool canEnableNfc: RemoteServiceModel.canEnableNfc
		id: startButton
		buttonColor: running ? "red" : "green"
		anchors.top: text.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.margins: Constants.component_spacing
		enabled: canEnableNfc || RemoteServiceModel.runnable || running
		onClicked: {
			if (canEnableNfc) {
				qmlExtension.showSettings("android.settings.NFC_SETTINGS")
			} else {
				var newRunning = !running;
				RemoteServiceModel.running = newRunning
			}
		}
		text: {
			settingsModel.translationTrigger; // Bind this evaluation to the trigger.

			if (canEnableNfc) {
				return qsTr("Enable NFC");
			} else if (running) {
				return qsTr("Stop remote service");
			} else {
				return qsTr("Start remote service");
			}
		}
		onRunningChanged: {
			navBar.lockedAndHidden = running
		}
	}

	GButton {
		id: pairingButton
		anchors.top: startButton.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.margins: Constants.component_spacing
		text: qsTr("Start pairing") + settingsModel.translationTrigger
		opacity: 0
		enabled: opacity === 1
		onClicked: popup.open()
	}

	Item {
		id: statusText

		anchors.top: startButton.bottom
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Constants.component_spacing
		anchors.topMargin: Constants.component_spacing * 2

		Text {
			id: error
			width: text.width

			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.horizontalCenter: parent.horizontalCenter
			horizontalAlignment: Text.AlignHCenter
			font.pixelSize: Utils.dp(16)
			font.bold: true
			color: "red"
			wrapMode: Text.WordWrap
			visible: !RemoteServiceModel.runnable
			text: RemoteServiceModel.errorMessage;
		}

		Item {
			id: connectedText
			anchors.fill: parent
			opacity: 0

			Text {
				id: headText
				anchors.top: connectedText.top
				anchors.horizontalCenter: parent.horizontalCenter
				font.pixelSize: Constants.header_font_size
				font.weight: Font.Bold
				color: Constants.blue

				text: qsTr("Card access in progress") + settingsModel.translationTrigger;
			}
			Text {
				id: subText
				color: Constants.secondary_text
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.pixelSize: Constants.normal_font_size
				anchors.top: headText.bottom
				anchors.topMargin: Utils.dp(10)
				anchors.horizontalCenter: parent.horizontalCenter
				width: connectedText.width * 0.8
				wrapMode: Text.WordWrap

				text: qsTr("Please pay attention to the display on your other device %1.").arg("\"" + RemoteServiceModel.connectedClientDeviceName + "\"") + settingsModel.translationTrigger;
			}

			states: [
				State { name: "UNCONNECTED"; when: RemoteServiceModel.running && !RemoteServiceModel.connected
					PropertyChanges { target: connectedText; opacity: 0 }
					PropertyChanges { target: pairingButton; opacity: 1 }
				},
				State { name: "CONNECTED"; when: RemoteServiceModel.running && RemoteServiceModel.connected
					PropertyChanges { target: connectedText; opacity: 1 }
					PropertyChanges { target: pairingButton; opacity: 0 }
				}
			]
			transitions: [
				Transition {
					NumberAnimation {
						property: "opacity"
						duration: 500
						easing.type: Easing.InOutQuad
					}
				}
			]
		}
	}

	states: [
		State { name: "OFF"; when: !RemoteServiceModel.running
			PropertyChanges { target: pairingButton; opacity: 0 }
			AnchorChanges { target: statusText; anchors.top: startButton.bottom }
		},
		State { name: "ON"; when: RemoteServiceModel.running
			PropertyChanges { target: pairingButton; opacity: 1 }
			AnchorChanges { target: statusText; anchors.top: pairingButton.bottom }
		}
	]
	transitions: [
		Transition {
			NumberAnimation {
				property: "opacity"
				duration: 500
				easing.type: Easing.InOutQuad
			}
			AnchorAnimation {
				duration: 500
			}
		}
	]
}
