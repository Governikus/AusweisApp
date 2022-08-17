/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Text {
	id: border

	property Item scope: parent
	property bool isOnLightBackground: true

	width: height
	horizontalAlignment: Text.AlignHCenter

	anchors.top: parent.top
	anchors.left: parent.left

	visible: scope.activeFocus && plugin.showFocusIndicator

	text: "✱"
	color: isOnLightBackground ? Style.color.focus_indicator : Style.color.focus_indicator_inverse
	font.pixelSize: Style.dimens.hint_font_size
}
