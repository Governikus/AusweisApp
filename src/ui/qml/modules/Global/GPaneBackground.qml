/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Style

Rectangle {
	id: root

	property bool drawShadow: true
	property real shadowScale: 1.025

	border.color: Style.color.pane.border.basic
	border.width: Style.dimens.border_width
	color: Style.color.pane.background.basic
	radius: Style.dimens.pane_radius

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software && drawShadow

		effect: GDropShadow {
			shadowScale: root.shadowScale
		}
	}
}
