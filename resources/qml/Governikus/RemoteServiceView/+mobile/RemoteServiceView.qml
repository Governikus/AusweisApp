/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.RemoteServiceModel

FlickableSectionPage {
	id: baseItem

	enableTileStyle: false

	//: LABEL ANDROID IOS
	title: qsTr("Card reader")

	navigationAction: NavigationAction {
		action: RemoteServiceModel.running ? NavigationAction.Action.Cancel : NavigationAction.Action.None

		onClicked: RemoteServiceModel.setRunning(false)
	}
	states: [
		State {
			name: "PAIRING"
			when: RemoteServiceModel.running && RemoteServiceModel.isPairing

			PropertyChanges {
				knownDevicesContainer.visible: false
				pairingCode.visible: true
				paringCodeLink.visible: true
			}
		},
		State {
			name: "CONNECTED_OR_STOPPED"
			when: !RemoteServiceModel.running || (RemoteServiceModel.running && RemoteServiceModel.connectedToPairedDevice)

			PropertyChanges {
				networkPermissionText.visible: false
				wifiInfo.visible: false
			}
		}
	]

	Connections {
		function onFireIsRunningChanged() {
			setLockedAndHidden(RemoteServiceModel.running);
		}

		target: RemoteServiceModel
	}
	RemoteServiceController {
		id: controller

		stackView: baseItem.stackView
	}
	GOptionsContainer {
		containerPadding: Constants.pane_padding
		//: LABEL ANDROID IOS
		title: !ApplicationModel.wifiEnabled ? qsTr("WiFi not active") :
		//: LABEL ANDROID IOS
		RemoteServiceModel.canEnableNfc ? qsTr("NFC not active") :
		//: LABEL ANDROID IOS
		!RemoteServiceModel.runnable ? qsTr("Remote service not available") :
		//: LABEL ANDROID IOS
		RemoteServiceModel.connectedToPairedDevice ? qsTr("Card access in progress") :
		//: LABEL ANDROID IOS
		RemoteServiceModel.isPairing ? qsTr("Waiting for pairing") :
		//: LABEL ANDROID IOS
		RemoteServiceModel.running ? qsTr("Waiting for connection") : ""

		ColumnLayout {
			spacing: Constants.component_spacing
			width: parent.width

			TintableIcon {
				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.text_spacing
				source: "qrc:///images/phone_to_pc.svg"
				sourceSize.height: Style.dimens.medium_icon_size
				tintColor: Style.color.image
			}
			GText {
				id: infoText

				readonly property string currentPin: RemoteServiceModel.psk
				//: INFO ANDROID IOS
				readonly property string enterCodeString: qsTr("Enter the pairing code %1 in the %2 on your other device.")

				Accessible.name: text
				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.text_spacing
				horizontalAlignment: Text.AlignHCenter
				//: INFO ANDROID IOS
				text: qsTr("You can use this Smartphone as a card reader for the %1 on other devices e.g. a laptop.\n\nTo do this you first have to pair that device with this smartphone.").arg(Qt.application.name)
				textStyle: RemoteServiceModel.runnable ? Style.text.normal : Style.text.normal_warning

				states: [
					State {
						when: !RemoteServiceModel.runnable && RemoteServiceModel.errorMessage !== ""

						PropertyChanges {
							infoText.text: RemoteServiceModel.errorMessage
						}
					},
					State {
						when: RemoteServiceModel.running && RemoteServiceModel.connectedToPairedDevice

						PropertyChanges {
							infoText.text: RemoteServiceModel.connectionInfo
						}
					},
					State {
						when: RemoteServiceModel.isPairing

						PropertyChanges {
							infoText.Accessible.name: infoText.enterCodeString.arg(infoText.currentPin.split("").join(" ")).arg(Qt.application.name)
							infoText.text: infoText.enterCodeString.arg(infoText.currentPin).arg(Qt.application.name)
						}
					},
					State {
						when: !RemoteServiceModel.running && knownDeviceList.count > 0

						PropertyChanges {
							//: INFO ANDROID IOS
							infoText.text: qsTr("Allow a connection with paired devices to use this Smartphone as a card reader or pair another device.")
						}
					},
					State {
						when: RemoteServiceModel.running && knownDeviceList.count > 0

						PropertyChanges {
							//: INFO ANDROID IOS
							infoText.text: qsTr("Paired devices may use this Smartphone as a card reader now.")
						}
					},
					State {
						when: RemoteServiceModel.running

						PropertyChanges {
							//: INFO ANDROID IOS
							infoText.text: qsTr("Waiting for connection from a paired device...")
						}
					}
				]
			}
			GText {
				id: pairingCode

				readonly property string currentPin: RemoteServiceModel.psk.toString()

				Accessible.ignored: true
				Layout.alignment: Qt.AlignHCenter
				Layout.topMargin: Constants.component_spacing
				horizontalAlignment: Text.AlignHCenter

				//: LABEL ANDROID IOS
				text: qsTr("Pairing code: <b>%1</b>").arg(RemoteServiceModel.isPairing ? currentPin : "0000")
				textStyle: Style.text.headline
				visible: false
			}
			MoreInformationLink {
				id: paringCodeLink

				Layout.alignment: Qt.AlignCenter
				Layout.topMargin: Constants.component_spacing
				//: LABEL ANDROID IOS
				text: qsTr("Where do I enter the pairing code?")
				visible: false

				onClicked: push(pairingCodeInfoView)

				Component {
					id: pairingCodeInfoView

					PairingCodeInfoView {
						text: paringCodeLink.text

						onNavActionClicked: pop()
					}
				}
			}
		}
	}
	GOptionsContainer {
		id: knownDevicesContainer

		Layout.topMargin: Constants.component_spacing
		containerPadding: Constants.pane_padding
		//: LABEL ANDROID IOS
		title: qsTr("Paired Devices")
		visible: RemoteServiceModel.runnable && knownDeviceList.count > 0

		ColumnLayout {
			id: knownDevices

			spacing: Constants.text_spacing
			width: parent.width

			Repeater {
				id: knownDeviceList

				model: RemoteServiceModel.allDevices

				delegate: DevicesListDelegate {
					highlightTitle: isLastAddedDevice
					linkQualityVisible: false
					title: remoteDeviceName
				}
			}
			GButton {
				//: LABEL ANDROID IOS
				Accessible.name: qsTr("Start pairing of a new device")
				Layout.alignment: Qt.AlignLeft
				Layout.topMargin: knownDevices.spacing
				background: null
				icon.source: "qrc:///images/material_add.svg"
				padding: 0
				//: LABEL ANDROID IOS
				text: qsTr("Pair new device")
				textStyle: Style.text.link
				tintIcon: true
				visible: !RemoteServiceModel.isPairing && !RemoteServiceModel.running

				onClicked: RemoteServiceModel.setRunning(!RemoteServiceModel.running, !RemoteServiceModel.isPairing)
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	RemoteServiceWifiInfo {
		id: wifiInfo

		Layout.fillWidth: true
		Layout.topMargin: Constants.component_spacing
	}
	LocalNetworkInfo {
		id: networkPermissionText

		Layout.bottomMargin: Constants.text_spacing
		Layout.topMargin: Constants.component_spacing
		visible: RemoteServiceModel.requiresLocalNetworkPermission
	}
	GProgressBar {
		id: progressBar

		Layout.fillWidth: true
		Layout.topMargin: Constants.component_spacing
		value: RemoteServiceModel.percentage
		visible: progressText.visible
	}
	GText {
		id: progressText

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.text_spacing
		text: RemoteServiceModel.displayText
		visible: text !== ""
	}
	GButton {
		id: pairConnectButton

		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Constants.component_spacing
		enabled: !RemoteServiceModel.isStarting
		visible: text !== ""

		states: [
			State {
				when: !ApplicationModel.wifiEnabled

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Enable WiFi")

					pairConnectButton.onClicked: ApplicationModel.enableWifi()
				}
			},
			State {
				when: RemoteServiceModel.canEnableNfc

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Enable NFC")

					pairConnectButton.onClicked: ApplicationModel.showSettings(ApplicationModel.SETTING_NFC)
				}
			},
			State {
				when: RemoteServiceModel.runnable && knownDeviceList.count > 0 && !RemoteServiceModel.isPairing && !RemoteServiceModel.running

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Allow connection")

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(true)
				}
			},
			State {
				when: RemoteServiceModel.runnable && knownDeviceList.count < 1 && !RemoteServiceModel.isPairing

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Pair device")

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(true, true)
				}
			},
			State {
				when: RemoteServiceModel.isPairing

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Cancel pairing")
					pairConnectButton.visible: true

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(false, false)
				}
			}
		]

		onClicked: RemoteServiceModel.setRunning(true, !RemoteServiceModel.isPairing)
	}
}
