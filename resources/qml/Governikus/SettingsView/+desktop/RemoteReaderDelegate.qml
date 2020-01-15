/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
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
		SettingsModel.translationTrigger

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
						return qsTr("Click to pair") + SettingsModel.translationTrigger
					}

					return remoteDeviceStatus
				}
			}
		}

		Row {
			Layout.preferredHeight: iconHeight

			spacing: Constants.component_spacing

			Image {
				source: {
					if (!isNetworkVisible && isPaired) {
						return "qrc:///images/icon_remote_inactive.svg"
					}

					if (linkQualityInPercent < 25) {
						return "qrc:///images/icon_remote_0.svg"
					}
					if (linkQualityInPercent < 50) {
						return "qrc:///images/icon_remote_25.svg"
					}
					if (linkQualityInPercent < 75) {
						return "qrc:///images/icon_remote_50.svg"
					}
					if (linkQualityInPercent < 100) {
						return "qrc:///images/icon_remote_75.svg"
					}

					return "qrc:///images/icon_remote_100.svg"
				}

				sourceSize.height: iconHeight
				fillMode: Image.PreserveAspectFit
			}

			Image {
				id: removeIcon

				visible: isPaired

				source: "qrc:///images/trash_icon.svg"
				sourceSize.height: iconHeight
				fillMode: Image.PreserveAspectFit

				MouseArea {
					id: trashMouse

					anchors.fill: parent

					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor
					onClicked: unpairDevice(deviceId)

					ToolTip.delay: 500
					ToolTip.visible: trashMouse.containsMouse
					ToolTip.text: qsTr("Remove remote device") + SettingsModel.translationTrigger
				}
			}

			Item {
				id: spacer

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
