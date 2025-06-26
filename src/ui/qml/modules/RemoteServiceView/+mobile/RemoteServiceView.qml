/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View
import Governikus.Workflow

FlickableSectionPage {
	id: root

	readonly property int nfcState: ApplicationModel.nfcState
	readonly property bool notRunningNoPairingMode: !RemoteServiceModel.running && !RemoteServiceModel.isPairing
	readonly property bool runnableNoPairedDevice: RemoteServiceModel.runnable && knownDeviceList.count === 0 && !RemoteServiceModel.isPairing
	readonly property bool runningInPairingMode: RemoteServiceModel.running && RemoteServiceModel.isPairing

	enableTileStyle: false
	spacing: Style.dimens.pane_spacing

	//: LABEL ANDROID IOS
	title: qsTr("Card reader")

	navigationAction: NavigationAction {
		action: RemoteServiceModel.running ? NavigationAction.Action.Cancel : NavigationAction.Action.None

		onClicked: RemoteServiceModel.setRunning(false)
	}
	states: [
		State {
			name: "PAIRING"
			when: root.runningInPairingMode

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
			}
		}
	]

	Connections {
		function onFireIsRunningChanged() {
			root.setLockedAndHidden(RemoteServiceModel.running);
		}

		target: RemoteServiceModel
	}
	RemoteServiceController {
		id: controller

		stackView: root.stackView
	}
	ColumnLayout {
		Layout.topMargin: -root.spacing
		spacing: root.spacing
		visible: root.nfcState !== ApplicationModel.NfcState.UNAVAILABLE

		GText {
			Layout.alignment: Qt.AlignHCenter
			horizontalAlignment: Text.AlignHCenter
			//: LABEL ANDROID IOS
			text: !ApplicationModel.wifiEnabled ? qsTr("WiFi not active") :
			//: LABEL ANDROID IOS
			RemoteServiceModel.canEnableNfc ? qsTr("NFC not active") :
			//: LABEL ANDROID IOS
			!RemoteServiceModel.runnable ? qsTr("NFC is not available") :
			//: LABEL ANDROID IOS
			RemoteServiceModel.connectedToPairedDevice ? qsTr("Card access in progress") :
			//: LABEL ANDROID IOS
			RemoteServiceModel.isPairing ? qsTr("Waiting for pairing") :
			//: LABEL ANDROID IOS
			RemoteServiceModel.running ? qsTr("Card reader ready") :
			//: LABEL ANDROID IOS
			root.runnableNoPairedDevice ? qsTr("No device paired") :
			//: LABEL ANDROID IOS
			RemoteServiceModel.runnable && knownDeviceList.count > 0 && root.notRunningNoPairingMode ? qsTr("Card reader not active") : ""
			textStyle: Style.text.headline
			visible: text !== ""
		}
		TintableIcon {
			id: pairingIcon

			Layout.alignment: Qt.AlignHCenter
			source: "qrc:///images/phone_to_pc.svg"
			sourceSize.height: Style.dimens.header_icon_size
			tintColor: Style.color.image
			visible: knownDeviceList.count === 0 || root.runningInPairingMode
		}
		AnimationLoader {
			Layout.alignment: Qt.AlignHCenter
			animated: false
			symbol: RemoteServiceModel.running ? Symbol.CHECK : Symbol.ERROR
			type: AnimationLoader.WAIT_FOR_SAC
			visible: !pairingIcon.visible
		}
		GText {
			id: subline

			textStyle: Style.text.subline

			states: [
				State {
					when: root.runnableNoPairedDevice

					PropertyChanges {
						//: LABEL ANDROID IOS
						subline.text: qsTr("Use this smartphone as a card reader for a paired device")
					}
				},
				State {
					when: RemoteServiceModel.runnable && knownDeviceList.count > 0 && root.notRunningNoPairingMode

					PropertyChanges {
						//: LABEL ANDROID IOS
						subline.text: qsTr("Activate the card reader")
					}
				},
				State {
					when: RemoteServiceModel.running && !RemoteServiceModel.isPairing

					PropertyChanges {
						//: LABEL ANDROID IOS
						subline.text: qsTr("Paired devices may use this Smartphone as a card reader now.")
					}
				},
				State {
					when: root.runningInPairingMode

					PropertyChanges {
						//: LABEL ANDROID IOS
						subline.text: qsTr("Enter the pairing code")
					}
				}
			]
		}
		GText {
			id: infoText

			readonly property string currentPin: RemoteServiceModel.psk
			//: INFO ANDROID IOS %1 is replaced with the pairing code, %2 with the name "AusweisApp"
			readonly property string enterCodeString: qsTr("Enter the pairing code \"%1\" in the %2 on your other device. Both devices have to be on the same network (e.g. WiFi).")

			//: INFO ANDROID IOS
			text: qsTr("You can use this Smartphone as a card reader for the %1 on other devices e.g. a laptop.\n\nTo do this you first have to pair that device with this smartphone.").arg(Qt.application.name)

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
					when: !RemoteServiceModel.running && knownDeviceList.count > 0

					PropertyChanges {
						//: INFO ANDROID IOS
						infoText.text: qsTr("This allows the paired devices to use this smartphone as a card reader.")
					}
				},
				State {
					when: root.runningInPairingMode

					PropertyChanges {
						infoText.Accessible.name: infoText.enterCodeString.arg(infoText.currentPin.split("").join(" ")).arg(Qt.application.name)
						infoText.text: infoText.enterCodeString.arg(infoText.currentPin).arg(Qt.application.name)
					}
				},
				State {
					when: RemoteServiceModel.running && !RemoteServiceModel.isPairing && knownDeviceList.count > 0

					PropertyChanges {
						infoText.text: "%1\n%2".arg(
						//: INFO ANDROID IOS
						qsTr("To do this, start a process on a paired device.")).arg(
						//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
						qsTr("Both devices have to be on the same network (e.g. WiFi)."))
					}
				}
			]
		}
	}
	GText {
		id: pairingCodeHeader

		Accessible.ignored: true
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter

		//: LABEL ANDROID IOS
		text: qsTr("Pairing code:")
		textStyle: Style.text.headline
		visible: pairingCode.visible
	}
	GText {
		id: pairingCode

		readonly property string currentPin: RemoteServiceModel.psk

		Accessible.ignored: true
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter
		text: RemoteServiceModel.isPairing ? currentPin : "0000"
		textStyle: Style.text.headline
		visible: false
	}
	GButton {
		id: pairConnectButton

		Layout.alignment: Qt.AlignHCenter
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

					pairConnectButton.onClicked: ApplicationModel.showSettings(ApplicationModel.Settings.NFC)
				}
			},
			State {
				when: RemoteServiceModel.runnable && knownDeviceList.count > 0 && root.notRunningNoPairingMode

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Activate card reader")

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(true)
				}
			},
			State {
				when: root.runnableNoPairedDevice

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
			},
			State {
				when: RemoteServiceModel.running

				PropertyChanges {
					//: LABEL ANDROID IOS
					pairConnectButton.text: qsTr("Stop card reader")

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(false, false)
				}
			}
		]

		onClicked: RemoteServiceModel.setRunning(true, !RemoteServiceModel.isPairing)
	}
	MoreInformationLink {
		id: paringCodeLink

		Layout.alignment: Qt.AlignCenter
		//: LABEL ANDROID IOS
		text: qsTr("Where do I enter the pairing code?")
		visible: false

		onClicked: root.push(pairingCodeInfoView)

		Component {
			id: pairingCodeInfoView

			PairingCodeInfoView {
				text: paringCodeLink.text

				onNavActionClicked: root.pop()
			}
		}
	}
	NfcWorkflow {
		id: nfcWorkflow

		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.topMargin: -root.spacing
		bottomMargin: 0
		isRemoteWorkflow: true
		leftMargin: 0
		rightMargin: 0
		topMargin: 0
		visible: root.nfcState === ApplicationModel.NfcState.UNAVAILABLE

		onShowRemoteServiceSettings: root.push(remoteServiceSettings)

		Component {
			id: remoteServiceSettings

			RemoteServiceSettings {
				Component.onCompleted: RemoteServiceModel.startDetection()
				Component.onDestruction: RemoteServiceModel.stopDetection(true)
			}
		}
	}
	GOptionsContainer {
		id: knownDevicesContainer

		containerPadding: Style.dimens.pane_padding
		//: LABEL ANDROID IOS
		title: qsTr("Paired Devices")
		visible: RemoteServiceModel.runnable && knownDeviceList.count > 0 && !progressText.visible

		ColumnLayout {
			id: knownDevices

			spacing: Style.dimens.text_spacing
			width: parent.width

			Repeater {
				id: knownDeviceList

				model: RemoteServiceModel.allDevices

				delegate: DevicesListDelegate {
					Layout.fillWidth: true
					linkQualityVisible: false
					showSeparator: index < knownDeviceList.count - 1 || addPairingLink.visible
					titleColor: Style.color.textNormal.basic
				}
			}
			GLink {
				id: addPairingLink

				//: LABEL ANDROID IOS
				Accessible.name: qsTr("Start pairing of a new device")
				Layout.alignment: Qt.AlignLeft
				horizontalPadding: 0
				icon.source: "qrc:///images/material_add.svg"
				//: LABEL ANDROID IOS
				text: qsTr("Pair new device")
				tintIcon: true
				verticalPadding: 0
				visible: root.notRunningNoPairingMode

				onClicked: RemoteServiceModel.setRunning(!RemoteServiceModel.running, !RemoteServiceModel.isPairing)
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
	}
	LocalNetworkInfo {
		id: networkPermissionText

		visible: RemoteServiceModel.requiresLocalNetworkPermission
	}
	GProgressBar {
		Layout.fillWidth: true
		value: RemoteServiceModel.percentage
		visible: progressText.visible
	}
	GText {
		id: progressText

		Layout.alignment: Qt.AlignHCenter
		text: RemoteServiceModel.displayText
		visible: text !== ""
	}
}
