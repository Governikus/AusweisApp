/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.RemoteServiceView
import Governikus.Style
import Governikus.Type

Item {
	id: root

	signal pairDevice(var pDeviceId)
	signal showNoSacFoundInfo

	implicitHeight: column.implicitHeight
	implicitWidth: column.implicitWidth

	ReaderScanEnabler {
		pluginType: ReaderManagerPluginType.REMOTE_IFD
	}
	Column {
		id: column

		anchors.fill: parent
		spacing: Style.dimens.pane_spacing

		GPane {
			spacing: Style.dimens.pane_spacing
			//: LABEL DESKTOP
			title: qsTr("Paired devices")
			visible: availablePairedDevices.count > 0
			width: parent.width

			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)

			Repeater {
				id: availablePairedDevices

				model: RemoteServiceModel.availablePairedDevices

				delegate: RemoteReaderDelegate {
					Layout.fillWidth: true

					onFocusChanged: if (focus)
						Utils.positionViewAtItem(this)
					onUnpairDevice: pDeviceId => RemoteServiceModel.forgetDevice(pDeviceId)
				}
			}
		}
		GPane {
			spacing: Style.dimens.pane_spacing

			//: LABEL DESKTOP
			title: qsTr("Last connected")
			visible: unavailablePairedDevices.count > 0
			width: parent.width

			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)

			Repeater {
				id: unavailablePairedDevices

				model: RemoteServiceModel.unavailablePairedDevices

				delegate: RemoteReaderDelegate {
					Layout.fillWidth: true

					onFocusChanged: if (focus)
						Utils.positionViewAtItem(this)
					onUnpairDevice: pDeviceId => RemoteServiceModel.forgetDevice(pDeviceId)
				}
			}
		}
		GPane {
			spacing: Style.dimens.pane_spacing

			//: LABEL DESKTOP
			title: qsTr("Add pairing")
			width: parent.width

			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)

			Repeater {
				id: availableDevices

				model: RemoteServiceModel.availableDevicesInPairingMode

				delegate: RemoteReaderDelegate {
					Layout.fillWidth: true

					onFocusChanged: if (focus)
						Utils.positionViewAtItem(this)
					onPairDevice: pDeviceId => root.pairDevice(pDeviceId)
				}
			}
			PairingProcessInfo {
				Layout.fillWidth: true
				visible: availableDevices.count === 0

				onInfoLinkClicked: root.showNoSacFoundInfo()
			}
		}
	}
}
