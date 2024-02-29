/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	height: Math.max(circle1.maximumHeight, circle2.maximumHeight, circle3.maximumHeight)
	state: "1"
	width: d.stepWidth * 2 + height

	states: [
		State {
			name: "1"

			PropertyChanges {
				circle1.enabled: true
				circle2.enabled: false
				circle3.enabled: false
			}
		},
		State {
			name: "2"

			PropertyChanges {
				circle1.enabled: false
				circle2.enabled: true
				circle3.enabled: false
			}
		},
		State {
			name: "3"

			PropertyChanges {
				circle1.enabled: false
				circle2.enabled: false
				circle3.enabled: true
			}
		}
	]
	transitions: [
		Transition {
			SequentialAnimation {
				PauseAnimation {
					duration: 200
				}
				PropertyAction {
					property: "enabled"
					target: circle1
				}
				PropertyAction {
					property: "enabled"
					target: circle2
				}
				PropertyAction {
					property: "enabled"
					target: circle3
				}
			}
		}
	]

	QtObject {
		id: d

		readonly property int stepWidth: plugin.scaleFactor * 250
	}
	Rectangle {
		id: line1

		anchors.left: circle1.horizontalCenter
		anchors.leftMargin: Constants.component_spacing + circle1.maximumHeight / 2
		anchors.right: circle2.horizontalCenter
		anchors.rightMargin: Constants.component_spacing + circle2.maximumHeight / 2
		anchors.verticalCenter: parent.verticalCenter
		color: Style.color.border
		height: Style.dimens.border_width
		width: d.stepWidth
	}
	Rectangle {
		id: line2

		anchors.left: circle2.horizontalCenter
		anchors.leftMargin: Constants.component_spacing + circle2.maximumHeight / 2
		anchors.right: circle3.horizontalCenter
		anchors.rightMargin: Constants.component_spacing + circle3.maximumHeight / 2
		anchors.verticalCenter: parent.verticalCenter
		color: Style.color.border
		height: Style.dimens.border_width
		width: d.stepWidth
	}
	TextCircle {
		id: circle1

		anchors.horizontalCenter: parent.left
		anchors.verticalCenter: parent.verticalCenter
		text: "1"
	}
	TextCircle {
		id: circle2

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		text: "2"
	}
	TextCircle {
		id: circle3

		anchors.horizontalCenter: parent.right
		anchors.verticalCenter: parent.verticalCenter
		text: "3"
	}
}
