/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style
import Governikus.Type

Rectangle {
	id: root

	property color borderColor: Style.color.focus_indicator
	property Item framee: parent
	property real marginFactor: 1
	property Item scope: parent
	property real size: Style.dimens.border_width * 2

	anchors.fill: framee
	anchors.margins: marginFactor * -size * 2
	border.color: root.borderColor
	border.width: {
		if (Qt.platform.os !== "windows" && ApplicationModel.screenReaderRunning)
			return 0;
		if (scope.activeFocus && UiPluginModel.showFocusIndicator)
			return size;
		return 0;
	}
	color: Style.color.transparent
	radius: Math.min(height / 4, Style.dimens.control_radius)
}
