/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.MultiInfoView
import Governikus.Style
import Governikus.SettingsView
import Governikus.TitleBar
import Governikus.Type

BaseOnboardingView {
	id: root

	spacing: Style.dimens.pane_spacing

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Action.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	ReaderScanEnabler {
		pluginType: ReaderManagerPluginType.REMOTE_IFD
	}
	Heading {
		Layout.bottomMargin: Style.dimens.pane_spacing
		Layout.topMargin: -root.spacing
		//: DESKTOP
		text: qsTr("Set up smartphone as card reader")
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0

		GCollapsible {
			arrowToLeft: true
			backgroundColor: Style.color.transparent
			drawBottomCorners: true
			drawTopCorners: true
			//: DESKTOP
			selectionTitle: expanded ? "" : qsTr("Click the arrow to show.")
			startExpanded: true
			//: DESKTOP
			title: qsTr("Pairing instructions")

			PairingGuide {
			}
		}
	}
	Subheading {
		id: deviceListHeading

		//: DESKTOP
		text: qsTr("Available devices")
	}
	AvailableDevices {
	}
	Component {
		id: connectSacView

		ConnectSacView {
			progress: root.progress
			showSuccessView: false
			title: root.title

			titleBarSettings: TitleBarSettings {
				navigationAction: NavigationAction.Action.Back

				onNavigationActionClicked: root.pop()
			}

			onPairingFailed: root.pop()
			onPairingSuccessful: {
				root.pop();
				root.continueOnboarding();
			}
		}
	}

	component AvailableDevices: GPane {
		Layout.fillWidth: true

		Repeater {
			id: devicesInPairingMode

			model: RemoteServiceModel.availableDevicesInPairingMode

			delegate: RemoteReaderDelegate {
				Layout.fillWidth: true

				onFocusChanged: if (focus)
					Utils.positionViewAtItem(this)
				onPairDevice: pDeviceId => {
					if (RemoteServiceModel.rememberServer(pDeviceId)) {
						root.push(connectSacView);
					}
				}
			}
		}
		Repeater {
			id: availablePairedDevices

			model: RemoteServiceModel.availablePairedDevices

			delegate: RemoteReaderDelegate {
				Layout.fillWidth: true

				customContent: GButton {
					//: DESKTOP
					text: qsTr("Use device")

					onClicked: root.continueOnboarding()
					onFocusChanged: if (focus)
						root.positionViewAtItem(this)
				}

				onFocusChanged: if (focus)
					Utils.positionViewAtItem(this)
			}
		}
		RemoteReaderDelegate {
			Accessible.name: remoteDeviceName
			Accessible.role: Accessible.StaticText
			Layout.fillWidth: true
			deviceId: "dummyDeviceId"
			isNetworkVisible: true
			isPaired: false
			isPairing: false
			linkQualityInPercent: 0
			//: DESKTOP
			remoteDeviceName: qsTr("No device within reach")
			remoteDeviceStatus: ""
			visible: devicesInPairingMode.count === 0 && availablePairedDevices.count === 0

			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		MoreInformationLink {
			//: DESKTOP
			text: qsTr("My smartphone does not show up")

			onClicked: root.push(multiInfoView)
		}
		Component {
			id: multiInfoView

			MultiInfoView {
				progress: root.progress

				infoContent: MultiInfoData {
					contentType: MultiInfoData.Type.NO_SAC_FOUND
				}
				titleBarSettings: TitleBarSettings {
					navigationAction: NavigationAction.Action.Back
					startEnabled: false

					onNavigationActionClicked: root.pop()
				}
			}
		}
	}
	component FramedImage: Image {
		Layout.alignment: Qt.AlignHCenter
		sourceSize.width: UiPluginModel.scaleFactor * 228

		Rectangle {
			anchors.fill: parent
			border.color: Style.color.textSubline.basic_unchecked
			border.width: Style.dimens.pane_border_highlight_width
			color: Style.color.transparent
		}
	}
	component PairingGuide: ColumnLayout {
		spacing: Style.dimens.groupbox_spacing

		GText {
			//: DESKTOP %1/%3 are replaced with bold highlighting, %2 with the name AusweisApp
			text: "1. " + qsTr("Ensure that the %1%2%3 on the smartphone that you want to use as card reader is %1up to date%3.").arg("<b>").arg(Qt.application.name).arg("</b>")
		}
		GText {
			//: DESKTOP %1/%2 are replaced with bold highlighting
			text: "2. " + qsTr("Connect both devices to the %1same network%2, e.g. via WiFi.").arg("<b>").arg("</b>")
		}
		GText {
			//: DESKTOP %1/%2 are replaced with bold highlighting
			text: "3. " + qsTr("Select the %1Card reader%2 entry in the bottom menu bar on your smartphone.").arg("<b>").arg("</b>")
		}
		FramedImage {
			source: "qrc:///images/desktop/navigation_bar_%1.png".arg(SettingsModel.language)
		}
		GText {
			//: DESKTOP %1/%2 are replaced with bold highlighting
			text: "4. " + qsTr("Then select %1Pair device%2 or, if a device was already paired, %1Pair new device%2.").arg("<b>").arg("</b>")
		}
		RowLayout {
			Layout.alignment: Qt.AlignHCenter
			spacing: Style.dimens.pane_spacing

			FramedImage {
				source: "qrc:///images/desktop/pair_device_%1.png".arg(SettingsModel.language)
			}
			GText {
				Accessible.ignored: true
				//: DESKTOP
				text: qsTr("or")
			}
			FramedImage {
				source: "qrc:///images/desktop/pair_new_device_%1.png".arg(SettingsModel.language)
			}
		}
		GText {
			//: DESKTOP %1 will be replaced with the Available Devices list name
			text: "5. " + qsTr("Afterwards, a pairing code is displayed on your smartphone. You may then select the device in the %1 list to enter the pairing code.").arg("<b>" + deviceListHeading.text + "</b>")
		}
	}
}
