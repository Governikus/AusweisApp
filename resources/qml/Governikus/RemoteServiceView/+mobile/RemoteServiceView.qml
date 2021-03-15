/*
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction {
		state: RemoteServiceModel.running ? "cancel" : ""
		onClicked: RemoteServiceModel.setRunning(false)
	}

	//: LABEL ANDROID IOS
	title: qsTr("Remote service")

	Connections {
		target: RemoteServiceModel
		//: ERROR ANDROID IOS An error occurred while pairing the device.
		onFirePairingFailed: ApplicationModel.showFeedback(qsTr("Pairing failed. Please start a new pairing process on your other device and enter the shown pairing code."))
		onFirePairingCompleted: pairingButton.didPairInSaKSession = true
		onFireIsRunningChanged: {
			navBar.lockedAndHidden = RemoteServiceModel.running
			pairingButton.didPairInSaKSession = false
		}
	}

	RemoteServiceController {
		id: controller
	}

	Item {
		anchors {
			fill: parent
			margins: Constants.pane_padding
			bottomMargin: Constants.pane_padding + (navBar.lockedAndHidden ? Style.dimens.navigation_bar_height : 0)
			Behavior on bottomMargin { NumberAnimation { duration: Constants.animation_duration } }
		}

		ColumnLayout {
			height: parent.height
			width: Math.min(parent.width, Style.dimens.max_text_width)
			anchors.centerIn: parent

			spacing: 0

			Image {
				Layout.fillWidth: true
				Layout.maximumHeight: width * (sourceSize.height / sourceSize.width)

				source: "qrc:///images/phone_to_pc.svg"
				fillMode: Image.PreserveAspectFit
			}

			GText {
				Layout.fillWidth: true
				Layout.topMargin: Constants.component_spacing

				//: LABEL ANDROID IOS
				text: !ApplicationModel.wifiEnabled ? qsTr("WiFi needed")
					//: LABEL ANDROID IOS
					: RemoteServiceModel.canEnableNfc ? qsTr("NFC needed")
					//: LABEL ANDROID IOS
					: !RemoteServiceModel.runnable ? qsTr("Remote service not available")
					//: LABEL ANDROID IOS
					: RemoteServiceModel.connectedToPairedDevice ? qsTr("Card access in progress")
					//: LABEL ANDROID IOS
					: RemoteServiceModel.isPairing || RemoteServiceModel.running ? qsTr("Waiting for connection")
					//: LABEL ANDROID IOS
					: qsTr("Remote service ready")
				textStyle: Style.text.header_accent
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				Layout.fillWidth: true
				Layout.topMargin: Constants.text_spacing

				text: !RemoteServiceModel.runnable ? RemoteServiceModel.errorMessage
					: RemoteServiceModel.running && RemoteServiceModel.connectedToPairedDevice ? RemoteServiceModel.connectionInfo
					//: INFO ANDROID IOS
					: RemoteServiceModel.isPairing ? qsTr("Enter this code on your other device to use your smartphone as a card reader (SaC)")
					 //: INFO ANDROID IOS
					: RemoteServiceModel.running ? qsTr("Waiting for connection from a paired device...")
					 //: INFO ANDROID IOS
					: qsTr("Start the remote access, in order to make this smartphone visible and to use it as a card reader (SaC)")
				textStyle: RemoteServiceModel.runnable ? Style.text.normal_secondary : Style.text.normal_warning
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				id: pairingCode

				readonly property string currentPin: RemoteServiceModel.psk.toString()

				visible: false
				Layout.fillWidth: true
				Layout.topMargin: Constants.component_spacing

				//: LABEL ANDROID IOS
				Accessible.name: qsTr("Pairing code: %1").arg(currentPin.split("").join(" "));

				//: LABEL ANDROID IOS
				text: qsTr("Pairing code: <b>%1</b>").arg(RemoteServiceModel.isPairing ? currentPin : "0000")
				textStyle: Style.text.title_accent
				horizontalAlignment: Text.AlignHCenter
			}

			GSpacer { Layout.fillHeight: true }

			RowLayout {
				id: wifiInfo

				Layout.fillWidth: true
				Layout.topMargin: Constants.component_spacing

				spacing: Constants.text_spacing

				TintableIcon {
					source: "qrc:/images/info.svg"
					sourceSize.width: Style.dimens.small_icon_size
					tintColor: Style.text.normal_secondary.textColor
				}

				GText {
					Layout.fillWidth: true

					//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
					text: qsTr("Both of your devices have to be connected to the same WiFi.")
					textStyle: Style.text.normal_secondary
				}
			}

			GText {
				id: networkPermissionText

				visible: false
				Layout.fillWidth: true
				Layout.bottomMargin: Constants.text_spacing
				Layout.topMargin: Constants.component_spacing

				text: "%1<br><a href=\"#\"><center>%2</center></a>"
					//: INFO IOS Let user know to check the application settings for local network permission
					.arg(qsTr("To be able to use your smartphone as card reader (SaC), please make sure that access to the local network is allowed for %1.").arg(Qt.application.name))
					//: INFO IOS Link to application settings
					.arg(qsTr("Go to application settings"))
				textStyle: Style.text.normal_secondary
				textFormat: Text.RichText

				onLinkActivated: ApplicationModel.showSettings(ApplicationModel.SETTING_APP)
			}

			GButton {
				id: startButton

				readonly property int minButtonWidth: Math.max(Math.max(pairingButton.implicitWidth, startButton.implicitWidth), parent.width / 2)

				Layout.minimumWidth: startButton.minButtonWidth
				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.component_spacing

				buttonColor: (!ApplicationModel.wifiEnabled || RemoteServiceModel.canEnableNfc) ? Style.color.button : (RemoteServiceModel.running ? Constants.red : Constants.darkgreen)
				enabled: (RemoteServiceModel.canEnableNfc || RemoteServiceModel.runnable || RemoteServiceModel.running || !ApplicationModel.wifiEnabled) && !RemoteServiceModel.isStarting
				//: LABEL ANDROID IOS
				text: !ApplicationModel.wifiEnabled ? qsTr("Enable WiFi")
					//: LABEL ANDROID IOS
					: RemoteServiceModel.canEnableNfc ? qsTr("Enable NFC")
					//: LABEL ANDROID IOS
					: RemoteServiceModel.running ? qsTr("Stop remote service")
					//: LABEL ANDROID IOS
					: qsTr("Start remote service");

				onClicked: {
					if (!ApplicationModel.wifiEnabled) {
						ApplicationModel.enableWifi()
					} else if (RemoteServiceModel.canEnableNfc) {
						ApplicationModel.showSettings(ApplicationModel.SETTING_NFC)
					} else {
						RemoteServiceModel.setRunning(!RemoteServiceModel.running)
					}
				}
			}

			GButton {
				id: pairingButton

				property bool didPairInSaKSession: false

				visible: RemoteServiceModel.runnable
				Layout.minimumWidth: startButton.minButtonWidth
				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.component_spacing

				// Set opacity instead of visibility to hide button so it keeps its size
				opacity: RemoteServiceModel.connectedToPairedDevice || didPairInSaKSession ? 0 : 1
				enabled: RemoteServiceModel.runnable && !RemoteServiceModel.connectedToPairedDevice && !RemoteServiceModel.isStarting && !didPairInSaKSession
				text: RemoteServiceModel.isPairing ?
					//: LABEL ANDROID IOS
					qsTr("Stop pairing") :
					//: LABEL ANDROID IOS
					qsTr("Start pairing")
				onClicked: RemoteServiceModel.setRunning(true, !RemoteServiceModel.isPairing)
			}

			states: [
				State { name: "PAIRING"; when: RemoteServiceModel.running && RemoteServiceModel.isPairing
					PropertyChanges { target: pairingCode; visible: true }
					PropertyChanges { target: wifiInfo; visible: true }
					PropertyChanges { target: networkPermissionText; visible: RemoteServiceModel.requiresLocalNetworkPermission }
				},
				State { name: "UNCONNECTED"; when: RemoteServiceModel.running && !RemoteServiceModel.connectedToPairedDevice
					PropertyChanges { target: pairingCode; visible: false }
					PropertyChanges { target: wifiInfo; visible: true }
					PropertyChanges { target: networkPermissionText; visible: RemoteServiceModel.requiresLocalNetworkPermission }
				},
				State { name: "CONNECTED_OR_STOPPED"; when: !RemoteServiceModel.running || (RemoteServiceModel.running && RemoteServiceModel.connectedToPairedDevice)
					PropertyChanges { target: pairingCode; visible: false }
					PropertyChanges { target: wifiInfo; visible: false }
					PropertyChanges { target: networkPermissionText; visible: false }
				}
			]
		}
	}
}
