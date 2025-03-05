/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style

Item {
	id: root

	property bool checkedCondition: statefulControl.checked
	property color controlBackground: controlStyle.background.basic
	property color controlBorder: controlStyle.border.basic
	property color controlContent: controlStyle.content.basic
	property color controlPreferredPaneBackground: Style.color.transparent
	property ControlComponents controlStyle: Style.color.control
	property bool disabledCondition: !statefulControl.enabled
	property bool groupMember: false
	property bool hoveredCondition: statefulControl.hovered
	property color linkColor: linkStyle.basic
	property ComponentColors linkStyle: Style.color.linkBasic
	property color paneBackground: paneStyle.background.basic
	property color paneBorder: paneStyle.border.basic
	property CoreComponents paneStyle: Style.color.pane
	property bool pressedCondition: statefulControl.pressed
	required property var statefulControl
	property color textHeadline: Style.color.textHeadline.basic
	property color textNormal: Style.color.textNormal.basic
	property color textSubline: Style.color.textSubline.basic
	property color textTitle: Style.color.textTitle.basic

	states: [
		State {
			name: "disabled"
			when: root.disabledCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.disabled
				root.controlBorder: controlStyle.border.disabled
				root.controlContent: controlStyle.content.disabled
				root.linkColor: linkStyle.disabled
				root.paneBackground: paneStyle.background.disabled
				root.paneBorder: paneStyle.border.disabled
				root.textHeadline: Style.color.textHeadline.disabled
				root.textNormal: Style.color.textNormal.disabled
				root.textSubline: Style.color.textSubline.disabled
				root.textTitle: Style.color.textTitle.disabled
			}
		},
		State {
			name: "pressed"
			when: root.pressedCondition && !(root.groupMember && root.checkedCondition)

			PropertyChanges {
				root.controlBackground: controlStyle.background.pressed
				root.controlBorder: controlStyle.border.pressed
				root.controlContent: controlStyle.content.pressed
				root.controlPreferredPaneBackground: paneStyle.background.pressed
				root.linkColor: linkStyle.pressed
				root.paneBackground: paneStyle.background.pressed
				root.paneBorder: paneStyle.border.pressed
				root.textHeadline: Style.color.textHeadline.pressed
				root.textNormal: Style.color.textNormal.pressed
				root.textSubline: Style.color.textSubline.pressed
				root.textTitle: Style.color.textTitle.pressed
			}
		},
		State {
			name: "hovered"
			when: root.hoveredCondition && !(root.groupMember && root.checkedCondition)

			PropertyChanges {
				root.controlBackground: controlStyle.background.hovered
				root.controlBorder: controlStyle.border.hovered
				root.controlContent: controlStyle.content.hovered
				root.controlPreferredPaneBackground: paneStyle.background.hovered
				root.linkColor: linkStyle.hovered
				root.paneBackground: paneStyle.background.hovered
				root.paneBorder: paneStyle.border.hovered
				root.textHeadline: Style.color.textHeadline.hovered
				root.textNormal: Style.color.textNormal.hovered
				root.textSubline: Style.color.textSubline.hovered
				root.textTitle: Style.color.textTitle.hovered
			}
		},
		State {
			name: "checked"
			when: root.checkedCondition

			PropertyChanges {
				root.controlBackground: controlStyle.background.checked
				root.controlBorder: controlStyle.border.checked
				root.controlContent: controlStyle.content.checked
				root.linkColor: linkStyle.checked
				root.paneBackground: paneStyle.background.checked
				root.paneBorder: paneStyle.border.checked
				root.textHeadline: Style.color.textHeadline.checked
				root.textNormal: Style.color.textNormal.checked
				root.textSubline: Style.color.textSubline.checked
				root.textTitle: Style.color.textTitle.checked
			}
		}
	]
	transitions: [
		EaseInPressedTransition {
			target: root
		}
	]
}
