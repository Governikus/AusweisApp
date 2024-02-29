/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: root

	implicitHeight: topRectangle.height + bottomRectangle.height

	RoundedRectangle {
		id: topRectangle

		bottomLeftCorner: false
		bottomRightCorner: false
		color: Style.color.pane_sublevel
		height: bottomRectangle.height / 2
		radius: Style.dimens.titlepane_radius
		width: bottomRectangle.width - 2 * Style.dimens.titlepane_radius

		anchors {
			bottom: bottomRectangle.top
			horizontalCenter: root.horizontalCenter
		}
	}
	RoundedRectangle {
		id: bottomRectangle

		anchors.bottom: root.bottom
		borderColor: Style.color.pane_border
		bottomLeftCorner: false
		bottomRightCorner: false
		color: Style.color.background
		height: Math.ceil(Style.dimens.titlepane_radius)
		radius: height
		width: root.width

		layer {
			enabled: GraphicsInfo.api !== GraphicsInfo.Software

			effect: GDropShadow {
				autoPaddingEnabled: false
				paddingRect: Qt.rect(0, root.height, 0, 0)
				verticalOffset: -3
			}
		}
	}
}
