/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import Governikus.Style

Item {
	id: root

	property color control: Style.color.control
	property color control_border: Style.color.control_border
	property color control_content: Style.color.control_content
	property color control_preferredPaneBackground: Style.color.transparent
	property bool disabledCondition: !statefulControl.enabled
	property bool hoveredCondition: root.overwriteHovered || statefulControl.hovered && Constants.is_desktop
	property bool overwriteHovered: false
	property bool overwritePressed: false
	property color pane: Style.color.pane
	property color pane_border: Style.color.pane_border
	property bool pressedCondition: root.overwritePressed || statefulControl.pressed
	required property var statefulControl
	property color text: Style.color.text
	property color text_headline: Style.color.text_headline
	property color text_subline: Style.color.text_subline
	property bool uncheckedCondition: statefulControl.checkable && !statefulControl.checked

	states: [
		State {
			name: "disabled"
			when: disabledCondition

			PropertyChanges {
				root.control: Style.color.control_disabled
				root.control_border: Style.color.control_border_disabled
				root.control_content: Style.color.control_content_disabled
				root.pane: Style.color.pane_disabled
				root.pane_border: Style.color.pane_border_disabled
				root.text: Style.color.text_disabled
				root.text_headline: Style.color.text_headline_disabled
				root.text_subline: Style.color.text_subline_disabled
			}
		},
		State {
			name: "pressed"
			when: pressedCondition

			PropertyChanges {
				root.control: Style.color.control_pressed
				root.control_border: Style.color.control_border_pressed
				root.control_content: Style.color.control_content_pressed
				root.control_preferredPaneBackground: Style.color.pane_pressed
				root.pane: Style.color.pane_pressed
				root.pane_border: Style.color.pane_border_pressed
				root.text: Style.color.text_pressed
				root.text_headline: Style.color.text_headline_pressed
				root.text_subline: Style.color.text_subline_pressed
			}
		},
		State {
			name: "hovered"
			when: hoveredCondition

			PropertyChanges {
				root.control: Style.color.control_hovered
				root.control_border: Style.color.control_border_hovered
				root.control_content: Style.color.control_content_hovered
				root.control_preferredPaneBackground: Style.color.pane_hovered
				root.pane: Style.color.pane_hovered
				root.pane_border: Style.color.pane_border_hovered
				root.text: Style.color.text_hovered
				root.text_headline: Style.color.text_headline_hovered
				root.text_subline: Style.color.text_subline_hovered
			}
		},
		State {
			name: "unchecked"
			when: uncheckedCondition

			PropertyChanges {
				root.control: Style.color.control_unchecked
				root.control_border: Style.color.control_border_unchecked
				root.control_content: Style.color.control_content_unchecked
			}
		}
	]
	transitions: [
		EaseInPressedTransition {
			target: root
		}
	]
}
