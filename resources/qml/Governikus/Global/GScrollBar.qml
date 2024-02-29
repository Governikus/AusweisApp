/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Style

ScrollBar {
	id: baseItem

	property bool autohide: !Constants.is_desktop
	property alias color: handler.color
	property bool highlighted: false

	function highlight() {
		highlighted = true;
		highlightTimer.restart();
	}

	Accessible.ignored: true
	minimumSize: Style.dimens.minimumScrollBarSize

	// Using only ScrollBar.AsNeeded leads to the scrollbar becoming visible when highlighted
	policy: size < 1.0 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

	contentItem: Item {
		implicitHeight: 100
		implicitWidth: Style.dimens.scrollbar_width + Style.dimens.scrollbar_padding_horizontal
		opacity: (!autohide || active || highlighted) ? 1.0 : 0.0

		Behavior on opacity {
			NumberAnimation {
				duration: Constants.animation_duration
				easing.type: Easing.InOutCubic
			}
		}

		Rectangle {
			id: handler

			anchors.left: parent.left
			border.color: colors.control_border
			border.width: 2 * Style.dimens.border_width
			color: colors.control
			height: parent.height
			radius: width / 2
			width: Style.dimens.scrollbar_width
		}
	}

	onPolicyChanged: if (policy === ScrollBar.AlwaysOn)
		highlight()

	StatefulColors {
		id: colors

		statefulControl: baseItem
		uncheckedCondition: false
	}
	Timer {
		id: highlightTimer

		onTriggered: baseItem.highlighted = false
	}
}
