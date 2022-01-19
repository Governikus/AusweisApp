/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

GText {
	readonly property var reCriticalMessage: new RegExp("[0-9]{3,} C")
	readonly property var reWarningMessage: new RegExp("[0-9]{3,} W")
	readonly property bool isCriticalMessage: reCriticalMessage.test(display)
	readonly property bool isWarningMessage: reWarningMessage.test(display)

	property real pixelSize: textStyle.textSize

	font.family: plugin.fixedFontFamily
	textStyle: Style.text.hint
	font.pixelSize: pixelSize
	textFormat: Text.PlainText
	color: isCriticalMessage ? Style.color.warning_text : (isWarningMessage ? Style.color.accent : textStyle.textColor)
	text: "%1 %2".arg(origin).arg(message)
}

