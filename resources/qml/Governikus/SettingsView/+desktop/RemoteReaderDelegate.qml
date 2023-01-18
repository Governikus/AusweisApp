/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

Item {
	id: root

	property int iconHeight: ApplicationModel.scaleFactor * 80

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)

	Accessible.name: {
		var msg = qsTr("Smartphone named \"%1\"").arg(remoteDeviceName) + subtext.text;
		if (isPaired) {
			return msg + qsTr("Press space to unpair the smartphone \"%1\".").arg(remoteDeviceName);
		}
		return msg + qsTr("Press space to pair the smartphone \"%1\".").arg(remoteDeviceName);
	}
	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	implicitHeight: rowLayout.implicitHeight

	Keys.onSpacePressed: isPaired ? unpairDevice(deviceId) : pairDevice(deviceId)

	FocusFrame {
	}
	RowLayout {
		id: rowLayout
		spacing: Constants.component_spacing
		width: parent.width

		Column {
			Layout.fillWidth: true

			GText {
				elide: Text.ElideRight
				maximumLineCount: 1
				text: remoteDeviceName
				textStyle: Style.text.header
				width: parent.width
			}
			GText {
				id: subtext
				text: {
					if (!isPaired) {
						return qsTr("Click to pair");
					}
					return remoteDeviceStatus;
				}
				textStyle: Style.text.normal
				width: parent.width
			}
		}
		Row {
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
				source: "qrc:///images/material_delete.svg"
				sourceSize.height: iconHeight
				tintColor: Style.color.accent
				visible: isPaired

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
		visible: !isPaired

		onClicked: pairDevice(deviceId)
	}
}
