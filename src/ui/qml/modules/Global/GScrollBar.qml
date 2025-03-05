/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Style

ScrollBar {
	id: root

	property bool autohide: !Style.is_layout_desktop
	property alias borderWidth: customContent.border.width
	property alias color: customContent.color
	property bool highlighted: false

	function highlight() {
		highlighted = true;
		highlightTimer.restart();
	}

	Accessible.ignored: true
	bottomPadding: Style.dimens.scrollbar_padding_vertical
	minimumSize: Style.dimens.minimumScrollBarSize
	// Using only ScrollBar.AsNeeded leads to the scrollbar becoming visible when highlighted
	policy: size < 1.0 ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff
	rightPadding: Style.dimens.scrollbar_padding_horizontal
	topPadding: Style.dimens.scrollbar_padding_vertical

	contentItem: Rectangle {
		id: customContent

		border.color: colors.controlBorder
		border.width: 2 * Style.dimens.border_width
		color: colors.controlBackground
		implicitHeight: 100
		implicitWidth: Style.dimens.scrollbar_width
		opacity: (!root.autohide || root.active || root.highlighted) ? 1.0 : 0.0
		radius: width / 2

		Behavior on opacity {
			NumberAnimation {
				duration: Style.animation_duration
				easing.type: Easing.InOutCubic
			}
		}
	}

	onPolicyChanged: if (policy === ScrollBar.AlwaysOn)
		highlight()

	StatefulColors {
		id: colors

		checkedCondition: false
		controlStyle: Style.color.controlScrollbar
		statefulControl: root
	}
	Timer {
		id: highlightTimer

		onTriggered: root.highlighted = false
	}
}
