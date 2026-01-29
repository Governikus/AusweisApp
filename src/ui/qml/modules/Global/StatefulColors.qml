/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style

Item {
	id: root

	property bool checkedCondition: statefulControl.checked
	property color controlBackground: controlStyle.background.basic_unchecked
	property color controlBorder: controlStyle.border.basic_unchecked
	property color controlContent: controlStyle.content.basic_unchecked
	property color controlPreferredPaneBackground: Style.color.transparent
	property ControlComponents controlStyle: Style.color.control
	property bool disabledCondition: !statefulControl.enabled
	property bool groupMember: false
	property bool hoveredCondition: statefulControl.hovered
	property color linkColor: linkStyle.basic_unchecked
	property ComponentColors linkStyle: Style.color.linkBasic
	property color paneBackground: paneStyle.background.basic_unchecked
	property color paneBorder: paneStyle.border.basic_unchecked
	property CoreComponents paneStyle: Style.color.pane
	property bool pressedCondition: statefulControl.pressed
	required property var statefulControl
	property color textHeadline: Style.color.textHeadline.basic_unchecked
	property color textNormal: Style.color.textNormal.basic_unchecked
	property color textSubline: Style.color.textSubline.basic_unchecked
	property color textTitle: Style.color.textTitle.basic_unchecked

	states: [
		State {
			name: "disabled_checked"
			when: root.disabledCondition && root.checkedCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.disabled_checked
				root.controlBorder: controlStyle.border.disabled_checked
				root.controlContent: controlStyle.content.disabled_checked
				root.linkColor: linkStyle.disabled_checked
				root.paneBackground: paneStyle.background.disabled_checked
				root.paneBorder: paneStyle.border.disabled_checked
				root.textHeadline: Style.color.textHeadline.disabled_checked
				root.textNormal: Style.color.textNormal.disabled_checked
				root.textSubline: Style.color.textSubline.disabled_checked
				root.textTitle: Style.color.textTitle.disabled_checked
			}
		},
		State {
			name: "disabled_unchecked"
			when: root.disabledCondition && !root.checkedCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.disabled_unchecked
				root.controlBorder: controlStyle.border.disabled_unchecked
				root.controlContent: controlStyle.content.disabled_unchecked
				root.linkColor: linkStyle.disabled_unchecked
				root.paneBackground: paneStyle.background.disabled_unchecked
				root.paneBorder: paneStyle.border.disabled_unchecked
				root.textHeadline: Style.color.textHeadline.disabled_unchecked
				root.textNormal: Style.color.textNormal.disabled_unchecked
				root.textSubline: Style.color.textSubline.disabled_unchecked
				root.textTitle: Style.color.textTitle.disabled_unchecked
			}
		},
		State {
			name: "pressed_checked"
			when: root.pressedCondition && root.checkedCondition && !root.groupMember

			PropertyChanges {
				root.controlBackground: controlStyle.background.pressed_checked
				root.controlBorder: controlStyle.border.pressed_checked
				root.controlContent: controlStyle.content.pressed_checked
				root.controlPreferredPaneBackground: paneStyle.background.pressed_checked
				root.linkColor: linkStyle.pressed_checked
				root.paneBackground: paneStyle.background.pressed_checked
				root.paneBorder: paneStyle.border.pressed_checked
				root.textHeadline: Style.color.textHeadline.pressed_checked
				root.textNormal: Style.color.textNormal.pressed_checked
				root.textSubline: Style.color.textSubline.pressed_checked
				root.textTitle: Style.color.textTitle.pressed_checked
			}
		},
		State {
			name: "pressed_unchecked"
			when: root.pressedCondition && !root.checkedCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.pressed_unchecked
				root.controlBorder: controlStyle.border.pressed_unchecked
				root.controlContent: controlStyle.content.pressed_unchecked
				root.controlPreferredPaneBackground: paneStyle.background.pressed_unchecked
				root.linkColor: linkStyle.pressed_unchecked
				root.paneBackground: paneStyle.background.pressed_unchecked
				root.paneBorder: paneStyle.border.pressed_unchecked
				root.textHeadline: Style.color.textHeadline.pressed_unchecked
				root.textNormal: Style.color.textNormal.pressed_unchecked
				root.textSubline: Style.color.textSubline.pressed_unchecked
				root.textTitle: Style.color.textTitle.pressed_unchecked
			}
		},
		State {
			name: "hovered_checked"
			when: root.hoveredCondition && root.checkedCondition && !root.groupMember

			PropertyChanges {
				root.controlBackground: controlStyle.background.hovered_checked
				root.controlBorder: controlStyle.border.hovered_checked
				root.controlContent: controlStyle.content.hovered_checked
				root.controlPreferredPaneBackground: paneStyle.background.hovered_checked
				root.linkColor: linkStyle.hovered_checked
				root.paneBackground: paneStyle.background.hovered_checked
				root.paneBorder: paneStyle.border.hovered_checked
				root.textHeadline: Style.color.textHeadline.hovered_checked
				root.textNormal: Style.color.textNormal.hovered_checked
				root.textSubline: Style.color.textSubline.hovered_checked
				root.textTitle: Style.color.textTitle.hovered_checked
			}
		},
		State {
			name: "hovered_unchecked"
			when: root.hoveredCondition && !root.checkedCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.hovered_unchecked
				root.controlBorder: controlStyle.border.hovered_unchecked
				root.controlContent: controlStyle.content.hovered_unchecked
				root.controlPreferredPaneBackground: paneStyle.background.hovered_unchecked
				root.linkColor: linkStyle.hovered_unchecked
				root.paneBackground: paneStyle.background.hovered_unchecked
				root.paneBorder: paneStyle.border.hovered_unchecked
				root.textHeadline: Style.color.textHeadline.hovered_unchecked
				root.textNormal: Style.color.textNormal.hovered_unchecked
				root.textSubline: Style.color.textSubline.hovered_unchecked
				root.textTitle: Style.color.textTitle.hovered_unchecked
			}
		},
		State {
			name: "checked"
			when: root.checkedCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.basic_checked
				root.controlBorder: controlStyle.border.basic_checked
				root.controlContent: controlStyle.content.basic_checked
				root.linkColor: linkStyle.basic_checked
				root.paneBackground: paneStyle.background.basic_checked
				root.paneBorder: paneStyle.border.basic_checked
				root.textHeadline: Style.color.textHeadline.basic_checked
				root.textNormal: Style.color.textNormal.basic_checked
				root.textSubline: Style.color.textSubline.basic_checked
				root.textTitle: Style.color.textTitle.basic_checked
			}
		}
	]
	transitions: [
		EaseInPressedTransition {
			target: root
		}
	]
}
