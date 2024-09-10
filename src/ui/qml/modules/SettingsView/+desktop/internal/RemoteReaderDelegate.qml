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
import Governikus.Type

RoundedRectangle {
	id: root

	property int iconHeight: Style.dimens.icon_size

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)

	Accessible.name: {
		//: INFO DESKTOP Name and status of remote device. %1 is replaced with the name, %2 with the status
		let msg = qsTr("Smartphone named \"%1\". %2. ").arg(remoteDeviceName).arg(subtext.text);
		if (isPaired && !isPairing) {
			//: INFO DESKTOP Text for activation action if the device is paired.
			return msg + qsTr("Press space to unpair the smartphone \"%1\".").arg(remoteDeviceName);
		}
		//: INFO DESKTOP Text for activation action if the device is unpaired.
		return msg + qsTr("Press space to pair the smartphone \"%1\".").arg(remoteDeviceName);
	}
	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	color: Style.color.paneSublevel.background.basic
	implicitHeight: rowLayout.implicitHeight + 2 * rowLayout.anchors.margins
	implicitWidth: rowLayout.implicitWidth + 2 * rowLayout.anchors.margins

	Keys.onSpacePressed: (isPaired && !isPairing && ApplicationModel.isScreenReaderRunning()) ? unpairDevice(deviceId) : pairDevice(deviceId)

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
				textFormat: Text.PlainText
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

				activeFocusOnTab: !ApplicationModel.isScreenReaderRunning()
				source: "qrc:///images/trash_icon.svg"
				sourceSize.height: iconHeight
				tintColor: Style.color.image
				visible: isPaired && !isPairing

				Keys.onSpacePressed: unpairDevice(deviceId)

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
				FocusFrame {
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
