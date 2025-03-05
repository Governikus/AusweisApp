/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

RoundedRectangle {
	id: root

	property alias customContent: customContentLoader.sourceComponent
	required property var deviceId
	readonly property bool hasCustomContent: customContentLoader.status !== Loader.Null
	required property bool isNetworkVisible
	required property bool isPaired
	required property bool isPairing
	required property int linkQualityInPercent
	required property string remoteDeviceName
	required property string remoteDeviceStatus

	signal pairDevice(var pDeviceId)
	signal unpairDevice(var pDeviceId)

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
	activeFocusOnTab: ApplicationModel.isScreenReaderRunning
	color: Style.color.paneSublevel.background.basic
	implicitHeight: rowLayout.implicitHeight + 2 * rowLayout.anchors.margins
	implicitWidth: rowLayout.implicitWidth + 2 * rowLayout.anchors.margins

	Keys.onSpacePressed: {
		if (!ApplicationModel.isScreenReaderRunning) {
			return;
		}
		if (hasCustomContent) {
			return;
		}

		(isPaired && !isPairing) ? unpairDevice(deviceId) : pairDevice(deviceId);
	}

	RowLayout {
		id: rowLayout

		anchors.fill: parent
		anchors.margins: Style.dimens.pane_padding
		spacing: Style.dimens.pane_spacing

		LinkQualityAnimation {
			Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
			inactive: !root.isNetworkVisible && root.isPaired
			percent: root.linkQualityInPercent
			size: Style.dimens.medium_icon_size
		}
		Column {
			Layout.fillWidth: true

			GText {
				activeFocusOnTab: false
				elide: Text.ElideRight
				maximumLineCount: 1
				text: root.remoteDeviceName
				textFormat: Text.PlainText
				textStyle: Style.text.headline
				width: parent.width
			}
			GText {
				id: subtext

				activeFocusOnTab: false
				text: root.remoteDeviceStatus
				visible: text !== ""
				width: parent.width
			}
		}
		GButton {
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			activeFocusOnTab: !ApplicationModel.isScreenReaderRunning
			//: LABEL DESKTOP
			text: qsTr("Pair")
			visible: root.isPairing && !root.hasCustomContent

			onClicked: root.pairDevice(root.deviceId)
		}
		TintableIcon {
			id: removeIcon

			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			activeFocusOnTab: !ApplicationModel.isScreenReaderRunning
			source: "qrc:///images/trash_icon.svg"
			sourceSize.height: Style.dimens.icon_size
			tintColor: Style.color.image
			visible: root.isPaired && !root.isPairing && !root.hasCustomContent

			Keys.onSpacePressed: root.unpairDevice(root.deviceId)

			MouseArea {
				id: trashMouse

				ToolTip.delay: Style.toolTipDelay
				ToolTip.text: qsTr("Remove remote device")
				ToolTip.visible: trashMouse.containsMouse
				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor
				hoverEnabled: true

				onClicked: root.unpairDevice(root.deviceId)
			}
			FocusFrame {
			}
			GSpacer {
				height: removeIcon.height
				visible: !trashMouse.visible && root.isPairing
				width: removeIcon.width
			}
		}
		Loader {
			id: customContentLoader

			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			visible: root.hasCustomContent
		}
	}
}
