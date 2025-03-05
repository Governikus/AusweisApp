/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: root

	readonly property int shadowHeight: visible ? 15 : 0

	RoundedRectangle {
		id: background

		bottomLeftCorner: false
		bottomRightCorner: false

		Behavior on color {
			ColorAnimation {
				duration: Style.animation_duration
			}
		}

		layer {
			enabled: GraphicsInfo.api !== GraphicsInfo.Software

			effect: GDropShadow {
				shadowOpacity: 0.2
				shadowScale: 0.975
				shadowVerticalOffset: -root.shadowHeight
			}
		}
		anchors {
			fill: parent
			topMargin: root.shadowHeight
		}
	}
}
