/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style
import Governikus.Type

Text {
	id: root

	property Item scope: parent

	anchors.left: parent.left
	anchors.top: parent.top
	color: Style.color.focus_indicator
	font.pixelSize: Style.dimens.text
	horizontalAlignment: Text.AlignHCenter
	text: "✱"
	visible: scope.activeFocus && UiPluginModel.showFocusIndicator
	width: height
}
