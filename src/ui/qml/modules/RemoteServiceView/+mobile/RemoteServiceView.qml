/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
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

	//: MOBILE
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
		stackView: root.stackView
	}
	ColumnLayout {
		Layout.alignment: Style.scanPatternAlignment
		Layout.topMargin: -root.spacing
		spacing: root.spacing
		visible: root.nfcState !== ApplicationModel.NfcState.UNAVAILABLE

		Heading {
			//: MOBILE
			text: !ApplicationModel.wifiEnabled ? qsTr("WiFi not active") :
			//: MOBILE
			RemoteServiceModel.canEnableNfc ? qsTr("NFC not active") :
			//: MOBILE
			!RemoteServiceModel.runnable ? qsTr("NFC is not available") :
			//: MOBILE
			RemoteServiceModel.connectedToPairedDevice ? qsTr("Card access in progress") :
			//: MOBILE
			RemoteServiceModel.isPairing ? qsTr("Waiting for pairing") :
			//: MOBILE
			RemoteServiceModel.running ? qsTr("Card reader ready") :
			//: MOBILE
			root.runnableNoPairedDevice ? qsTr("No device paired") :
			//: MOBILE
			RemoteServiceModel.runnable && knownDeviceList.count > 0 && root.notRunningNoPairingMode ? qsTr("Card reader not active") : ""
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
			symbol: RemoteServiceModel.running ? Symbol.Type.CHECK : Symbol.Type.ERROR
			type: AnimationLoader.Type.WAIT_FOR_SAC
			visible: !pairingIcon.visible
		}
		Subheading {
			id: subline

			states: [
				State {
					when: root.runnableNoPairedDevice

					PropertyChanges {
						//: MOBILE
						subline.text: qsTr("Use this smartphone as a card reader for a paired device")
					}
				},
				State {
					when: RemoteServiceModel.runnable && knownDeviceList.count > 0 && root.notRunningNoPairingMode

					PropertyChanges {
						//: MOBILE
						subline.text: qsTr("Activate the card reader")
					}
				},
				State {
					when: RemoteServiceModel.running && !RemoteServiceModel.isPairing

					PropertyChanges {
						//: MOBILE
						subline.text: qsTr("Paired devices may use this Smartphone as a card reader now.")
					}
				},
				State {
					when: root.runningInPairingMode

					PropertyChanges {
						//: MOBILE
						subline.text: qsTr("Enter the pairing code")
					}
				}
			]
		}
		GText {
			id: infoText

			readonly property string currentPin: RemoteServiceModel.psk
			//: MOBILE %1 is replaced with the pairing code, %2 with the name "AusweisApp"
			readonly property string enterCodeString: qsTr("Enter the pairing code \"%1\" in the %2 on your other device. Both devices have to be on the same network (e.g. WiFi).")

			//: MOBILE
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
						//: MOBILE
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
						//: MOBILE
						qsTr("To do this, start a process on a paired device.")).arg(
						//: MOBILE The remote service is active. Hint that both devices need to be connected to the same network.
						qsTr("Both devices have to be on the same network (e.g. WiFi)."))
					}
				}
			]
		}
	}
	GText {
		Accessible.ignored: true
		Layout.alignment: Qt.AlignHCenter
		horizontalAlignment: Text.AlignHCenter

		//: MOBILE
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
					//: MOBILE
					pairConnectButton.text: qsTr("Enable WiFi")

					pairConnectButton.onClicked: ApplicationModel.enableWifi()
				}
			},
			State {
				when: RemoteServiceModel.canEnableNfc

				PropertyChanges {
					//: MOBILE
					pairConnectButton.text: qsTr("Enable NFC")

					pairConnectButton.onClicked: ApplicationModel.showSettings(ApplicationModel.Settings.NFC)
				}
			},
			State {
				when: RemoteServiceModel.runnable && knownDeviceList.count > 0 && root.notRunningNoPairingMode

				PropertyChanges {
					//: MOBILE
					pairConnectButton.text: qsTr("Activate card reader")

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(true)
				}
			},
			State {
				when: root.runnableNoPairedDevice

				PropertyChanges {
					//: MOBILE
					pairConnectButton.text: qsTr("Pair device")

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(true, true)
				}
			},
			State {
				when: RemoteServiceModel.isPairing

				PropertyChanges {
					//: MOBILE
					pairConnectButton.text: qsTr("Cancel pairing")
					pairConnectButton.visible: true

					pairConnectButton.onClicked: RemoteServiceModel.setRunning(false, false)
				}
			},
			State {
				when: RemoteServiceModel.running

				PropertyChanges {
					//: MOBILE
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
		//: MOBILE
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
		containerSpacing: Style.dimens.text_spacing
		//: MOBILE
		title: qsTr("Paired Devices")
		visible: RemoteServiceModel.runnable && knownDeviceList.count > 0 && !progressText.visible

		Repeater {
			id: knownDeviceList

			model: RemoteServiceModel.allDevices

			delegate: DevicesListDelegate {
				Layout.fillWidth: true
				linkQualityVisible: false
				titleColor: Style.color.textNormal.basic_unchecked
			}
		}
		GSeparator {
			Layout.fillWidth: true
			visible: addPairingLink.visible
		}
		GLink {
			id: addPairingLink

			//: MOBILE
			Accessible.name: qsTr("Start pairing of a new device")
			Layout.alignment: Qt.AlignLeft
			horizontalPadding: 0
			icon.source: "qrc:///images/material_add.svg"
			//: MOBILE
			text: qsTr("Pair new device")
			tintIcon: true
			verticalPadding: 0
			visible: root.notRunningNoPairingMode

			onClicked: RemoteServiceModel.setRunning(!RemoteServiceModel.running, !RemoteServiceModel.isPairing)
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
		//: MOBILE Name of an progress indicator during the pairing process read by screen readers
		Accessible.name: qsTr("Pairing progress")
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
