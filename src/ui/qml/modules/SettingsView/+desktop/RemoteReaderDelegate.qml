/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Animations
import Governikus.Global
import Governikus.Style

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

	//: DESKTOP Name of remote device. %1 is replaced with the name.
	Accessible.name: qsTr("Smartphone named \"%1\".").arg(remoteDeviceName)
	Accessible.role: Accessible.Grouping
	color: Style.color.paneSublevel.background.basic_unchecked
	implicitHeight: rowLayout.implicitHeight + 2 * rowLayout.anchors.margins
	implicitWidth: rowLayout.implicitWidth + 2 * rowLayout.anchors.margins

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
				Accessible.ignored: true
				elide: Text.ElideRight
				maximumLineCount: 1
				text: root.remoteDeviceName
				textFormat: Text.PlainText
				textStyle: Style.text.headline
				width: parent.width
			}
			GText {
				//: DESKTOP Status of remote device. %1 is replaced with the status.
				Accessible.name: qsTr("Status: \"%1\".").arg(text)
				text: root.remoteDeviceStatus
				visible: text !== ""
				width: parent.width
			}
		}
		GButton {
			id: delegate

			readonly property bool shouldPair: root.isPairing && !root.hasCustomContent
			readonly property bool shouldUnpair: root.isPaired && !root.isPairing && !root.hasCustomContent

			//: DESKTOP Text of pairing button, %1 will be Pair/Unpair and %2 is replaced with device name
			Accessible.name: qsTr("%1 device \"%2\"").arg(text).arg(root.remoteDeviceName)
			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			icon.source: shouldUnpair ? "qrc:///images/trash_icon.svg" : ""
			//: DESKTOP
			text: shouldPair ? qsTr("Pair") :
			//: DESKTOP
			shouldUnpair ? qsTr("Unpair") : ""
			tintIcon: true
			visible: text !== ""

			onClicked: shouldPair ? root.pairDevice(root.deviceId) : shouldUnpair ? root.unpairDevice(root.deviceId) : {}
		}
		Loader {
			id: customContentLoader

			Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
			visible: root.hasCustomContent
		}
	}
}
