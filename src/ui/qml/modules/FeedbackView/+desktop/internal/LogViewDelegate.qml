/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type

GText {
	id: root

	required property string level
	required property string message
	required property string origin

	function copyTextToClipboard() {
		ApplicationModel.setClipboardText(root.text);
		//: INFO DESKTOP Toast message used to confirm the copy of a log entry.
		ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."));
	}

	color: level === "C" ? Style.color.warning : (level === "W" ? Style.color.textSubline.basic : textStyle.textColor)
	focusFrameVisible: false
	font.bold: activeFocus
	font.family: UiPluginModel.fixedFontFamily
	lineHeight: 1.0
	lineHeightMode: Text.ProportionalHeight
	text: "%1 %2".arg(origin).arg(message)
	textFormat: Text.PlainText
	wrapMode: Text.Wrap

	Action {
		enabled: root.activeFocus
		shortcut: StandardKey.Copy

		onTriggered: root.copyTextToClipboard()
	}
	MouseArea {
		anchors.fill: parent

		onPressAndHold: root.copyTextToClipboard()
	}
}
