/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Style 1.0


TextEdit {
	readonly property var textStyle: Style.text.hint

	Accessible.role: Accessible.StaticText
	Accessible.name: text

	readOnly: true
	selectByMouse: true
	selectedTextColor: Style.color.primary_text_inverse
	selectionColor: Style.color.accent
	color: level === "C" ? Style.color.warning_text : (level === "W" ? Style.color.accent : textStyle.textColor)
	font {
		family: plugin.fixedFontFamily
		pixelSize: textStyle.textSize
	}
	textFormat: Text.PlainText
	wrapMode: Text.Wrap
	text: "%1 %2".arg(origin).arg(message)
}
