/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

ColumnLayout {
	id: root

	property alias description: descriptionText.text
	required property bool isLastAddedDevice
	required property bool isSupported
	required property string remoteDeviceName
	required property string remoteDeviceStatus
	property alias showSeparator: separator.visible
	property alias titleColor: titleText.color

	Accessible.description: description
	//: MOBILE %1 is replaced with the device's name
	Accessible.name: qsTr("Device %1").arg(titleText.text)
	Accessible.role: Accessible.ListItem
	spacing: Style.dimens.text_spacing

	GSeparator {
		id: separator

		Layout.fillWidth: true
		visible: false
	}
	ColumnLayout {
		Layout.fillWidth: true
		spacing: 2

		GText {
			id: titleText

			Accessible.ignored: true
			elide: Text.ElideRight
			font.weight: root.isLastAddedDevice ? Style.font.bold : Style.font.normal
			maximumLineCount: 1
			text: root.remoteDeviceName + (root.isSupported ? "" : (" (" + root.remoteDeviceStatus + ")"))
			textFormat: Text.PlainText
			textStyle: Style.text.subline
		}
		GText {
			id: descriptionText

			Accessible.ignored: true
			elide: Text.ElideRight
			maximumLineCount: 1
			visible: text !== ""
		}
	}
}
