/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Rectangle {
	id: baseItem

	property Item framee: parent
	property Item scope: parent
	property real marginFactor: 1
	property bool isOnLightBackground: true

	readonly property real size: Math.max(ApplicationModel.scaleFactor * 4, 1)

	anchors.fill: framee
	anchors.margins: marginFactor * -size * 2
	radius: size * 2
	border.width: scope.activeFocus && plugin.showFocusIndicator ? size : 0;
	border.color: isOnLightBackground ? Style.color.focus_indicator : Style.color.focus_indicator_inverse
	color: Style.color.transparent
}
