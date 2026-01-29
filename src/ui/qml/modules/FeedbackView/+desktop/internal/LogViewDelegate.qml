/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
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
		//: DESKTOP Toast message used to confirm the copy of a log entry.
		ApplicationModel.showFeedback(qsTr("The log entry was copied to the clipboard."));
	}

	Accessible.role: Utils.useSpecialAppleTabRole(Accessible.StaticText)
	color: level === "C" ? Style.color.warning : (level === "W" ? Style.color.textSubline.basic_unchecked : textStyle.textColor)
	focusFrameVisible: false
	font.family: UiPluginModel.fixedFontFamily
	font.weight: activeFocus ? Style.font.bold : Style.font.normal
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
