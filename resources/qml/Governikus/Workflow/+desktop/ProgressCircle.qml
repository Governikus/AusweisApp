/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	height: selector.height
	state: "1"
	width: d.stepWidth * 4 + height

	states: [
		State {
			name: "1"

			PropertyChanges {
				enabled: true
				target: circle1
			}
			PropertyChanges {
				enabled: false
				target: circle2
			}
			PropertyChanges {
				enabled: false
				target: circle3
			}
			PropertyChanges {
				anchors.leftMargin: 0
				target: line
			}
		},
		State {
			name: "2"

			PropertyChanges {
				enabled: false
				target: circle1
			}
			PropertyChanges {
				enabled: true
				target: circle2
			}
			PropertyChanges {
				enabled: false
				target: circle3
			}
			PropertyChanges {
				anchors.leftMargin: -d.stepWidth
				target: line
			}
		},
		State {
			name: "3"

			PropertyChanges {
				enabled: false
				target: circle1
			}
			PropertyChanges {
				enabled: false
				target: circle2
			}
			PropertyChanges {
				enabled: true
				target: circle3
			}
			PropertyChanges {
				anchors.leftMargin: 2 * -d.stepWidth
				target: line
			}
		}
	]
	transitions: [
		Transition {
			PropertyAnimation {
				duration: 500
				easing.type: Easing.InOutCubic
				property: "anchors.leftMargin"
				target: line
			}
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

		readonly property int stepWidth: ApplicationModel.scaleFactor * 250
	}
	Rectangle {
		id: line
		anchors.left: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: Style.color.border
		height: ApplicationModel.scaleFactor * 8
		width: d.stepWidth * 2
	}
	TextCircle {
		id: circle1
		anchors.horizontalCenter: line.left
		anchors.verticalCenter: line.verticalCenter
		text: "1"
	}
	TextCircle {
		id: circle2
		anchors.horizontalCenter: line.horizontalCenter
		anchors.verticalCenter: line.verticalCenter
		text: "2"
	}
	TextCircle {
		id: circle3
		anchors.horizontalCenter: line.right
		anchors.verticalCenter: line.verticalCenter
		text: "3"
	}
	Rectangle {
		id: selector
		anchors.centerIn: parent
		border.color: Style.color.accent
		border.width: ApplicationModel.scaleFactor * 6
		color: Style.color.transparent
		height: circle1.height + ApplicationModel.scaleFactor * 40
		radius: height / 2
		width: height
	}
}
