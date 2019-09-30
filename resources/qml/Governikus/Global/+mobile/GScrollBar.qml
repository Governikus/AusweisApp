/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Style 1.0

ScrollBar {
	id: baseItem

	function highlight() {
		highlighted = true
		highlightTimer.restart()
	}

	property bool highlighted: false

	// Using only ScrollBar.AsNeeded leads to the scrollbar becoming visible when highlighted
	policy: size < 1.0 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
	minimumSize: Style.dimens.minimumScrollBarSize

	contentItem: Rectangle {
		implicitWidth: 3
		implicitHeight: 100
		radius: width / 2
		color: baseItem.pressed ? Style.color.accent : Constants.grey
		opacity: (active || highlighted) ? 1.0 : 0.0

		Behavior on opacity {
			NumberAnimation {
				duration: Constants.animation_duration
				easing.type: Easing.InOutCubic
			}
		}
	}

	onPolicyChanged: if (policy == ScrollBar.AlwaysOn) highlight()

	Timer {
		id: highlightTimer
		onTriggered: baseItem.highlighted = false
	}
}
