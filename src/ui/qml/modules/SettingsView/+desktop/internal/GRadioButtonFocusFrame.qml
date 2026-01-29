/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.Style
import Governikus.Type

Rectangle {
	id: root

	property bool checked: false
	property bool show: false

	anchors.fill: parent
	anchors.margins: -(border.width * 2)
	border.color: Style.color.focus_indicator
	border.width: Style.dimens.border_width * 2
	color: Style.color.transparent
	visible: {
		if (Qt.platform.os !== "windows" && ApplicationModel.screenReaderRunning)
			return false;
		if (show && UiPluginModel.showFocusIndicator)
			return true;
		return false;
	}

	// override top and bottom border corner of previous and next button in group
	Frame {
		border.color: Style.color.pane.background.basic_unchecked
		border.width: root.border.width

		// draw underlying button border to have also the left border
		Frame {
			border.color: root.checked ? Style.color.controlRadiobutton.border.basic_checked : Style.color.controlRadiobutton.border.basic_unchecked
			border.width: Style.dimens.border_width
		}
	}

	component Frame: Rectangle {
		anchors.fill: parent
		anchors.margins: (parent as Rectangle).border.width
		bottomLeftRadius: (parent as Rectangle).bottomLeftRadius
		bottomRightRadius: (parent as Rectangle).bottomRightRadius
		color: "transparent"
		topLeftRadius: (parent as Rectangle).topLeftRadius
		topRightRadius: (parent as Rectangle).topRightRadius
	}
}
