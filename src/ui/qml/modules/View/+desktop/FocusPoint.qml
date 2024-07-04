/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type

Text {
	id: border

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
