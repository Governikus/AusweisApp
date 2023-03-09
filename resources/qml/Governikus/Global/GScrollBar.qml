/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Style 1.0

ScrollBar {
	id: baseItem

	property bool autohide: !Constants.is_desktop
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

		Behavior on opacity  {
			NumberAnimation {
				duration: Constants.animation_duration
				easing.type: Easing.InOutCubic
			}
		}

		Rectangle {
			id: handler
			anchors.left: parent.left
			color: baseItem.pressed ? Style.color.button : Style.color.button_disabled
			height: parent.height
			radius: width / 2
			width: Style.dimens.scrollbar_width
		}
	}

	onPolicyChanged: if (policy === ScrollBar.AlwaysOn)
		highlight()

	Timer {
		id: highlightTimer
		onTriggered: baseItem.highlighted = false
	}
}
