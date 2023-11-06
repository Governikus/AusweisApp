/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Rectangle {
	id: baseItem

	property color borderColor: Style.color.focus_indicator
	property Item framee: parent
	property real marginFactor: 1
	property Item scope: parent
	readonly property real size: Math.max(plugin.scaleFactor * 4, 1)

	anchors.fill: framee
	anchors.margins: marginFactor * -size * 2
	border.color: baseItem.borderColor
	border.width: scope.activeFocus && plugin.showFocusIndicator ? size : 0
	color: Style.color.transparent
	radius: size * 2
}
