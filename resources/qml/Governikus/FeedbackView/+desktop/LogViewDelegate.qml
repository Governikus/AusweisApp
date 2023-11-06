/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel

GText {
	id: root

	function copyTextToClipboard() {
		ApplicationModel.setClipboardText(root.text);
		//: INFO DESKTOP Toast message used to confirm the copy of a log entry.
		ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."));
	}

	color: level === "C" ? Style.color.text_warning : (level === "W" ? Style.color.control : textStyle.textColor)
	font.bold: activeFocus
	font.family: plugin.fixedFontFamily
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
