/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0

Item {
	id: root

	signal unpairDevice(string pDeviceId)
	signal pairDevice(string pDeviceId)

	property int iconHeight: ApplicationModel.scaleFactor * 80

	implicitHeight: rowLayout.implicitHeight

	activeFocusOnTab: true
	Accessible.role: Accessible.Button
	Accessible.name: {
		var msg = qsTr("Smartphone named \"%1\"").arg(remoteDeviceName) + subtext.text
		if (isPaired) {
			return msg + qsTr("Press space to unpair the smartphone \"%1\".").arg(remoteDeviceName)
		}
		return msg + qsTr("Press space to pair the smartphone \"%1\".").arg(remoteDeviceName)
	}
	Keys.onSpacePressed: isPaired ? unpairDevice(deviceId) : pairDevice(deviceId)

	FocusFrame {
		borderColor: Style.color.focus_indicator
	}

	RowLayout {
		id: rowLayout

		width: parent.width

		spacing: Constants.component_spacing

		Column {
			Layout.fillWidth: true

			GText {
				width: parent.width

				textStyle: Style.text.header
				text: remoteDeviceName
				elide: Text.ElideRight
				maximumLineCount: 1
			}

			GText {
				id: subtext

				width: parent.width

				textStyle: Style.text.normal
				text: {
					if (!isPaired) {
						return qsTr("Click to pair")
					}

					return remoteDeviceStatus
				}
			}
		}

		Row {
			Layout.preferredHeight: iconHeight

			spacing: Constants.component_spacing

			LinkQuality {
				sourceSize.height: iconHeight

				inactive: !isNetworkVisible && isPaired
				percent: linkQualityInPercent
			}

			TintableIcon {
				id: removeIcon

				visible: isPaired

				source: "qrc:///images/material_delete.svg"
				sourceSize.height: iconHeight
				tintColor: Style.color.accent
				fillMode: Image.PreserveAspectFit

				MouseArea {
					id: trashMouse

					anchors.fill: parent

					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: unpairDevice(deviceId)

					ToolTip.delay: Constants.toolTipDelay
					ToolTip.visible: trashMouse.containsMouse
					ToolTip.text: qsTr("Remove remote device")
				}
			}

			GSpacer {
				visible: !removeIcon.visible

				height: removeIcon.height
				width: removeIcon.width
			}
		}
	}

	MouseArea {
		anchors.fill: parent

		visible: !isPaired
		onClicked: pairDevice(deviceId)
		cursorShape: Qt.PointingHandCursor
	}
}
