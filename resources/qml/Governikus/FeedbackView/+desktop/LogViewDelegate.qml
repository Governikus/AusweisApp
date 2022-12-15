/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0

TextEdit {
	readonly property var textStyle: Style.text.hint

	Accessible.name: text
	Accessible.role: Accessible.StaticText
	color: level === "C" ? Style.color.warning_text : (level === "W" ? Style.color.accent : textStyle.textColor)
	readOnly: true
	selectByMouse: true
	selectedTextColor: Style.color.primary_text_inverse
	selectionColor: Style.color.accent
	text: "%1 %2".arg(origin).arg(message)
	textFormat: Text.PlainText
	wrapMode: Text.Wrap

	font {
		family: plugin.fixedFontFamily
		pixelSize: textStyle.textSize
	}
}
