/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	readonly property int _stepWidth: 60

	height: 70
	state: "inactive"

	states: [
		State {
			name: "inactive"

			PropertyChanges {
				state: "inactive"
				target: tCircle1
			}
			PropertyChanges {
				state: "inactive"
				target: tCircle2
			}
			PropertyChanges {
				anchors.leftMargin: 0
				target: rec1
			}
		},
		State {
			name: "one"

			PropertyChanges {
				state: "active"
				target: tCircle1
			}
			PropertyChanges {
				state: "inactive"
				target: tCircle2
			}
			PropertyChanges {
				anchors.leftMargin: 0
				target: rec1
			}
		},
		State {
			name: "two"

			PropertyChanges {
				state: "inactive"
				target: tCircle1
			}
			PropertyChanges {
				state: "active"
				target: tCircle2
			}
			PropertyChanges {
				anchors.leftMargin: -_stepWidth
				target: rec1
			}
		}
	]
	transitions: [
		Transition {
			to: "two"

			PropertyAnimation {
				duration: 500
				easing.type: Easing.InOutCubic
				property: "anchors.leftMargin"
				target: rec1
			}
		}
	]

	Rectangle {
		id: rec1
		anchors.left: baseItem.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		color: Constants.blue
		height: 2
		width: _stepWidth
	}
	Rectangle {
		id: tCircle1
		anchors.horizontalCenter: rec1.left
		anchors.verticalCenter: rec1.verticalCenter
		border.color: Constants.white
		border.width: state === "active" ? 2 : 0
		color: Style.color.background
		height: width
		radius: width / 2
		state: "inactive"
		width: state === "active" ? 70 : 25 + border.width

		states: [
			State {
				name: "active"

				PropertyChanges {
					target: innerDisc
					width: 50
				}
			},
			State {
				name: "inactive"

				PropertyChanges {
					target: innerDisc
					width: 25
				}
			}
		]
		transitions: [
			Transition {
				to: "active"

				PropertyAnimation {
					duration: 1500
					easing.type: Easing.OutBounce
					property: "width"
					target: innerDisc
				}
			}
		]

		Rectangle {
			id: innerDisc
			anchors.centerIn: parent
			border.color: Constants.blue
			border.width: 1
			color: tCircle1.state === "active" ? Style.color.accent : Constants.white
			height: width
			radius: width / 2

			GText {
				Accessible.ignored: true
				anchors.centerIn: parent
				color: tCircle1.state === "active" ? Constants.white : Style.color.accent
				font.pixelSize: parent.height / 3
				text: "1"
				textStyle: Style.text.normal_highlight
			}
		}
	}
	TextCircle {
		id: tCircle2
		anchors.horizontalCenter: rec1.right
		anchors.verticalCenter: rec1.verticalCenter
		text: "2"
	}
}
