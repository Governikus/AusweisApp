/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel

Text {
	id: border

	property Item scope: parent

	anchors.left: parent.left
	anchors.top: parent.top
	color: Style.color.focus_indicator
	font.pixelSize: Style.dimens.text
	horizontalAlignment: Text.AlignHCenter
	text: "✱"
	visible: scope.activeFocus && plugin.showFocusIndicator
	width: height
}
