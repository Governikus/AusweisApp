/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.RemoteServiceView
import Governikus.View

RoundedRectangle {
	id: root

	property int iconHeight: Style.dimens.icon_size

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)

	Accessible.name: {
		let msg = qsTr("Smartphone named \"%1\"").arg(remoteDeviceName) + subtext.text;
		if (isPaired) {
			return msg + qsTr("Press space to unpair the smartphone \"%1\".").arg(remoteDeviceName);
		}
		return msg + qsTr("Press space to pair the smartphone \"%1\".").arg(remoteDeviceName);
	}
	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	color: Style.color.pane_sublevel
	implicitHeight: rowLayout.implicitHeight + 2 * rowLayout.anchors.margins
	implicitWidth: rowLayout.implicitWidth + 2 * rowLayout.anchors.margins

	Keys.onSpacePressed: isPaired ? unpairDevice(deviceId) : pairDevice(deviceId)

	FocusFrame {
	}
	RowLayout {
		id: rowLayout

		anchors.fill: parent
		anchors.margins: Constants.pane_padding
		spacing: Constants.component_spacing

		Column {
			Layout.fillWidth: true

			GText {
				elide: Text.ElideRight
				maximumLineCount: 1
				text: remoteDeviceName
				textStyle: Style.text.headline
				width: parent.width
			}
			GText {
				id: subtext

				text: remoteDeviceStatus
				width: parent.width
			}
		}
		Row {
			Layout.alignment: Qt.AlignVCenter
			Layout.preferredHeight: iconHeight
			spacing: Constants.component_spacing

			LinkQuality {
				inactive: !isNetworkVisible && isPaired
				percent: linkQualityInPercent
				sourceSize.height: iconHeight
			}
			TintableIcon {
				id: removeIcon

				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/trash_icon.svg"
				sourceSize.height: iconHeight
				tintColor: Style.color.image
				visible: isPaired && !isPairing

				MouseArea {
					id: trashMouse

					ToolTip.delay: Constants.toolTipDelay
					ToolTip.text: qsTr("Remove remote device")
					ToolTip.visible: trashMouse.containsMouse
					anchors.fill: parent
					cursorShape: Qt.PointingHandCursor
					hoverEnabled: true

					onClicked: unpairDevice(deviceId)
				}
			}
			GSpacer {
				height: removeIcon.height
				visible: !removeIcon.visible
				width: removeIcon.width
			}
		}
	}
	MouseArea {
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor
		visible: !trashMouse.visible

		onClicked: pairDevice(deviceId)
	}
}
