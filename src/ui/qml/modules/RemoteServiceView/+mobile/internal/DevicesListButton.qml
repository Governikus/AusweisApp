/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View

GAbstractButton {
	id: root

	property alias description: devicesListItem.description
	required property var deviceId
	required property int index
	required property bool isLastAddedDevice
	required property bool isNetworkVisible
	required property bool isPaired
	required property bool isSupported
	required property int linkQualityInPercent
	required property string remoteDeviceName
	required property string remoteDeviceStatus

	signal activate(var pIsSupported, var pDeviceId)

	Accessible.description: devicesListItem.description
	Accessible.name: devicesListItem.Accessible.name + ", " + linkQualityAnimation.Accessible.name
	Accessible.role: Accessible.Button

	contentItem: ColumnLayout {
		spacing: Style.dimens.text_spacing

		GSeparator {
			Layout.fillWidth: true
			visible: root.index > 0
		}
		Item {
			Layout.fillWidth: true
			implicitHeight: rowLayout.implicitHeight

			RowLayout {
				id: rowLayout

				anchors.fill: parent

				DevicesListItem {
					id: devicesListItem

					Accessible.ignored: true
					isLastAddedDevice: root.isLastAddedDevice
					isSupported: root.isSupported
					remoteDeviceName: root.remoteDeviceName
					remoteDeviceStatus: root.remoteDeviceStatus
				}
				GSpacer {
					Layout.fillWidth: true
				}
				LinkQualityAnimation {
					id: linkQualityAnimation

					Accessible.ignored: true
					inactive: !root.isNetworkVisible && root.isPaired
					percent: root.linkQualityInPercent

					MouseArea {
						anchors.fill: parent
					}
				}
			}
			FocusFrame {
				scope: root
			}
		}
	}

	Keys.onSpacePressed: clicked()
	onClicked: activate(isSupported, deviceId)
}
