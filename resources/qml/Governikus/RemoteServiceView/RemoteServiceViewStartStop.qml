/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


Item {
	id: baseItem

	readonly property int maxWidth: Math.min(width - 2 * Constants.component_spacing, Style.dimens.max_text_width)

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
		width: parent.maxWidth
		fillMode: Image.PreserveAspectFit
	}

	GText {
		id: text

		Accessible.role: Accessible.StaticText
		Accessible.name: text.text

		width: parent.maxWidth
		anchors.top: image.bottom
		anchors.margins: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
		text: qsTr("Please start the remote service in order to use your smartphone as a card reader with AusweisApp2."
				+ " Please note: Both your devices have to be connected to the same WiFi.")
				+ SettingsModel.translationTrigger
		textStyle: Style.text.normal_secondary
	}


	GButton {
		readonly property bool running: RemoteServiceModel.running
		readonly property bool canEnableNfc: RemoteServiceModel.canEnableNfc
		property bool serviceIsStarting: false
		id: startButton
		buttonColor: running ? Constants.red : Constants.green
		anchors.top: text.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.margins: Constants.component_spacing
		enabled: (canEnableNfc || RemoteServiceModel.runnable || running) && !serviceIsStarting
		onClicked: {
			if (canEnableNfc) {
				ApplicationModel.showSettings(ApplicationModel.SETTING_NFC)
			} else {
				if (!running) {
					serviceIsStarting = true
				}
				RemoteServiceModel.running = !running
			}
		}
		text: {
			SettingsModel.translationTrigger; // Bind this evaluation to the trigger.

			if (canEnableNfc) {
				//: LABEL ANDROID IOS
				return qsTr("Enable NFC");
			} else if (running) {
				//: LABEL ANDROID IOS
				return qsTr("Stop remote service");
			} else {
				//: LABEL ANDROID IOS
				return qsTr("Start remote service");
			}
		}
		onRunningChanged: {
			serviceIsStarting = false
			navBar.lockedAndHidden = running
		}
	}

	GButton {
		id: pairingButton
		anchors.top: startButton.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.margins: Constants.component_spacing
		//: LABEL ANDROID IOS
		text: qsTr("Start pairing") + SettingsModel.translationTrigger
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

		GText {
			id: error

			width: text.width
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.horizontalCenter: parent.horizontalCenter

			Accessible.role: Accessible.StaticText
			Accessible.name: error.text

			horizontalAlignment: Text.AlignHCenter
			visible: !RemoteServiceModel.runnable
			text: RemoteServiceModel.errorMessage
			textStyle: Style.text.normal_warning
		}

		Item {
			id: connectedText
			anchors.fill: parent
			opacity: 0

			GText {
				id: headText

				anchors.top: connectedText.top
				anchors.horizontalCenter: parent.horizontalCenter

				Accessible.role: Accessible.StaticText
				Accessible.name: headText.text

				//: LABEL ANDROID IOS
				text: qsTr("Card access in progress") + SettingsModel.translationTrigger;
				textStyle: Style.text.header_accent
			}
			GText {
				id: subText

				anchors.top: headText.bottom
				anchors.topMargin: 10
				anchors.horizontalCenter: parent.horizontalCenter
				width: connectedText.width * 0.8

				Accessible.role: Accessible.StaticText
				Accessible.name: subText.text

				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				text: RemoteServiceModel.connectionInfo
				textStyle: Style.text.normal_secondary
			}

			states: [
				State { name: "UNCONNECTED"; when: RemoteServiceModel.running && !RemoteServiceModel.connected
					PropertyChanges { target: connectedText; opacity: 0 }
					PropertyChanges { target: pairingButton; opacity: 1 }
					PropertyChanges { target: ApplicationModel; nfcRunning: false; restoreEntryValues: false }
				},
				State { name: "CONNECTED"; when: RemoteServiceModel.running && RemoteServiceModel.connected
					PropertyChanges { target: connectedText; opacity: 1 }
					PropertyChanges { target: pairingButton; opacity: 0 }
					PropertyChanges { target: ApplicationModel; nfcRunning: true; restoreEntryValues: false }
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
