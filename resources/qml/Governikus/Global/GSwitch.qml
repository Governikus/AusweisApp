/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Style
import Governikus.View

Switch {
	id: control

	property bool overwriteHovered: false
	property bool overwritePressed: false

	implicitHeight: indicator.implicitHeight
	implicitWidth: indicator.implicitWidth
	padding: 0

	// Empty item since we don't want the text
	contentItem: Item {
	}
	indicator: Rectangle {
		border.color: d.borderColor
		color: d.controlColor
		implicitHeight: implicitWidth / 2
		implicitWidth: Style.dimens.switch_width
		radius: height / 2

		Rectangle {
			id: ball

			readonly property int distanceBallBorder: 3

			anchors.verticalCenter: parent.verticalCenter
			color: d.contentColor
			height: parent.height - 2 * distanceBallBorder
			radius: height / 2
			width: height
			x: control.checked ? parent.width - width - distanceBallBorder : distanceBallBorder

			Behavior on x {
				NumberAnimation {
					duration: 200
					easing.type: Easing.InOutQuad
				}
			}
		}
	}

	Accessible.onPressAction: toggle()

	FocusFrame {
		radius: height / 2
	}
	Item {
		id: d

		property color borderColor: Style.color.control_border
		property color contentColor: Style.color.control_content
		property color controlColor: Style.color.control

		states: [
			State {
				name: "disabled"
				when: !control.enabled

				PropertyChanges {
					borderColor: Style.color.control_border_disabled
					contentColor: Style.color.control_content_disabled
					controlColor: Style.color.control_disabled
					target: d
				}
			},
			State {
				name: "hovered"
				when: control.overwriteHovered || control.hovered

				PropertyChanges {
					borderColor: Style.color.control_border_hover
					contentColor: Style.color.control_content_hover
					controlColor: Style.color.control_hover
					target: d
				}
			},
			State {
				name: "pressed"
				when: control.overwritePressed || control.pressed

				PropertyChanges {
					borderColor: Style.color.control_border_pressed
					contentColor: Style.color.control_content_pressed
					controlColor: Style.color.control_pressed
					target: d
				}
			},
			State {
				name: "unchecked"
				when: !control.checked

				PropertyChanges {
					borderColor: Style.color.control_border_unchecked
					contentColor: Style.color.control_content_unchecked
					controlColor: Style.color.control_unchecked
					target: d
				}
			}
		]
	}
}
