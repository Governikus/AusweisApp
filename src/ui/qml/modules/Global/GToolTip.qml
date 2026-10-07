/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick.Controls
import Governikus.Style
import Governikus.Type

ToolTip {
	readonly property TextStyle textStyle: Style.text.toolTip

	delay: Style.toolTipDelay
	font.family: UiPluginModel.fontFamily
	font.pixelSize: textStyle.textSize
	font.weight: textStyle.fontWeight
}
