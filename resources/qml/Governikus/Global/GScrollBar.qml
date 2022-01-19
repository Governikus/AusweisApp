/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Style 1.0

ScrollBar {
	id: baseItem

	function highlight() {
		highlighted = true
		highlightTimer.restart()
	}

	property bool highlighted: false
	property bool autohide: !Constants.is_desktop

	// Using only ScrollBar.AsNeeded leads to the scrollbar becoming visible when highlighted
	policy: size < 1.0 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
	minimumSize: Style.dimens.minimumScrollBarSize

	contentItem: Item {

		implicitWidth: Style.dimens.scrollbar_width + Style.dimens.scrollbar_padding_horizontal
		implicitHeight: 100

		opacity: (!autohide || active || highlighted) ? 1.0 : 0.0

		Behavior on opacity {
			NumberAnimation {
				duration: Constants.animation_duration
				easing.type: Easing.InOutCubic
			}
		}

		Rectangle {
			id: handler

			width: Style.dimens.scrollbar_width
			height: parent.height
			anchors.left: parent.left

			radius: width / 2
			color: baseItem.pressed ? Style.color.button : Style.color.button_disabled
		}
	}

	onPolicyChanged: if (policy == ScrollBar.AlwaysOn) highlight()

	Timer {
		id: highlightTimer
		onTriggered: baseItem.highlighted = false
	}
}
