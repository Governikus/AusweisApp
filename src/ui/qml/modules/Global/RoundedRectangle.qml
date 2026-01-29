/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style

Rectangle {
	id: root

	property bool bottomLeftCorner: true
	property bool bottomRightCorner: true
	property bool topLeftCorner: true
	property bool topRightCorner: true

	border.color: color
	border.width: Style.dimens.border_width
	bottomLeftRadius: bottomLeftCorner ? radius : 0
	bottomRightRadius: bottomRightCorner ? radius : 0
	color: Style.color.background
	radius: Style.dimens.pane_radius
	topLeftRadius: topLeftCorner ? radius : 0
	topRightRadius: topRightCorner ? radius : 0
}
