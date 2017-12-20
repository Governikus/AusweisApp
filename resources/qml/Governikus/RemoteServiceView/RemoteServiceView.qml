import QtQuick 2.7
import QtQuick.Controls 1.4

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.TechnologyInfo 1.0

SectionPage {
	leftTitleBarAction: TitleBarAction {
		state: remoteServiceModel.running ? "cancel" : ""
		onClicked: startButton.clicked()
	}

	headerTitleBarAction: TitleBarAction {
		text: qsTr("Smartphone as card reader") + settingsModel.translationTrigger;
		font.bold: true
	}

	readonly property int maxWidth: Math.min(width - 2 * Constants.component_spacing, Utils.dp(500))
	id: root

	RemoteServiceSettings {
		id: remoteSettings
		visible: false
	}

	RemoteServiceController {
			id: controller
		}

	RemoteServicePairingPopup {
		id: popup
		x: (root.width - width) / 2
		y: (root.height - height) / 2

		pin: remoteServiceModel.psk.toString()
		onPinChanged: {
			if (pin === "") {
				close()
			}
		}
		onVisibleChanged: remoteServiceModel.setPairing(visible)
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

		width: parent.maxWidth
		anchors.top: image.bottom
		anchors.margins: Constants.component_spacing
		anchors.horizontalCenter: parent.horizontalCenter

		text: qsTr("Please start the remote service in order to use your smartphone as a card reader with AusweisApp2."
				+ " Please note: Both your devices have to be connected to the same WiFi.")
				+ settingsModel.translationTrigger
		font.pixelSize: Constants.normal_font_size
		horizontalAlignment: Text.AlignJustify
		wrapMode: Text.WordWrap
	}


	GButton {
		readonly property bool running: remoteServiceModel.running
		readonly property bool canEnableNfc: remoteServiceModel.canEnableNfc
		id: startButton
		buttonColor: running ? "red" : "green"
		anchors.top: text.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.margins: Constants.component_spacing
		onClicked: {
			if (canEnableNfc) {
				qmlExtension.showSettings("android.settings.NFC_SETTINGS")
			} else {
				var newRunning = !running;
				remoteServiceModel.running = newRunning
				qmlExtension.keepScreenOn(newRunning)
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
		enabled: remoteServiceModel.runnable || canEnableNfc
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
			visible: !remoteServiceModel.runnable
			text: remoteServiceModel.errorMessage;
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
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				font.pixelSize: Constants.normal_font_size
				anchors.top: headText.bottom
				anchors.topMargin: Utils.dp(10)
				anchors.horizontalCenter: parent.horizontalCenter
				width: connectedText.width * 0.8
				wrapMode: Text.WordWrap

				text: qsTr("Please pay attention to the display on your other device.") + settingsModel.translationTrigger;
			}

			states: [
				State { name: "UNCONNECTED"; when: remoteServiceModel.running && !remoteServiceModel.connected
					PropertyChanges { target: connectedText; opacity: 0 }
					PropertyChanges { target: pairingButton; opacity: 1 }
				},
				State { name: "CONNECTED"; when: remoteServiceModel.running && remoteServiceModel.connected
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

	EnterPinView {
		id: enterPinView
		visible: false

		leftTitleBarAction: TitleBarAction {
			state: "cancel"
			onClicked: remoteServiceModel.cancelPasswordRequest()
		}

		onPinEntered: {
			numberModel.continueWorkflow()
		}
	}

	TechnologySwitchButton {
		id: switchTo
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		onClicked: firePush(remoteSettings, {})
		imageSource: "qrc:///images/android/navigation/remotesettings.svg"
		text: qsTr("Settings") + settingsModel.translationTrigger
		opacity: 1
		enabled: opacity === 1
	}

	states: [
		State { name: "OFF"; when: !remoteServiceModel.running
			PropertyChanges { target: pairingButton; opacity: 0 }
			PropertyChanges { target: switchTo; opacity: 1 }
		},
		State { name: "ON"; when: remoteServiceModel.running
			PropertyChanges { target: pairingButton; opacity: 1 }
			PropertyChanges { target: switchTo; opacity: 0 }
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
