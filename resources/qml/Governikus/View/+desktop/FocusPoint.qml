/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Text {
	id: border

	property bool isOnLightBackground: true
	property Item scope: parent

	anchors.left: parent.left
	anchors.top: parent.top
	color: isOnLightBackground ? Style.color.focus_indicator : Style.color.focus_indicator_inverse
	font.pixelSize: Style.dimens.hint_font_size
	horizontalAlignment: Text.AlignHCenter
	text: "✱"
	visible: scope.activeFocus && plugin.showFocusIndicator
	width: height
}
