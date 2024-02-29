/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global

Item {
	id: root

	readonly property int shadowHeight: visible ? 15 : 0

	RoundedRectangle {
		id: background

		bottomLeftCorner: false
		bottomRightCorner: false

		Behavior on color {
			ColorAnimation {
				duration: Constants.animation_duration
			}
		}

		layer {
			enabled: GraphicsInfo.api !== GraphicsInfo.Software

			effect: GDropShadow {
				shadowOpacity: 0.2
				shadowScale: 0.975
				verticalOffset: -root.shadowHeight
			}
		}
		anchors {
			fill: parent
			topMargin: root.shadowHeight
		}
	}
}
