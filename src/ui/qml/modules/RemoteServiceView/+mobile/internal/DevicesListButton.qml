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

	property alias description: listItem.description
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

	Accessible.role: Accessible.Button

	contentItem: ColumnLayout {
		spacing: Style.dimens.text_spacing

		GSeparator {
			Layout.fillWidth: true
			visible: root.index > 0
		}
		RowLayout {
			DevicesListItem {
				id: listItem

				isLastAddedDevice: root.isLastAddedDevice
				isSupported: root.isSupported
				remoteDeviceName: root.remoteDeviceName
				remoteDeviceStatus: root.remoteDeviceStatus
			}
			GSpacer {
				Layout.fillWidth: true
			}
			LinkQualityAnimation {
				inactive: !root.isNetworkVisible && root.isPaired
				percent: root.linkQualityInPercent

				MouseArea {
					anchors.fill: parent
				}
			}
		}
	}

	Keys.onSpacePressed: clicked()
	onClicked: activate(isSupported, deviceId)

	FocusFrame {
		marginFactor: 3
	}
}
