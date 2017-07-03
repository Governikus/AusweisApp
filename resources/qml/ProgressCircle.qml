import QtQuick 2.5
import QtQuick.Controls 1.4

import "global"

Item {
	id: baseItem
	height: Utils.dp(70)
	readonly property int _stepWidth: Utils.dp(60)

	Rectangle {
		id: rec1
		anchors.horizontalCenter: baseItem.horizontalCenter
		anchors.horizontalCenterOffset: _stepWidth
		anchors.verticalCenter: parent.verticalCenter
		width: 2 * _stepWidth
		height: Utils.dp(2)
		color: Constants.blue
	}

	Rectangle {
		id: tCircle1
		anchors.verticalCenter: rec1.verticalCenter
		anchors.horizontalCenter: rec1.left
		width: state === "active" ? Utils.dp(70) : Utils.dp(25) + border.width
		height: width
		radius: width / 2
		color: Constants.background_color
		border.color: "white"
		border.width: state === "active" ? 2 : 0

		state: "inactive"
		states: [
			State { name: "active"
				PropertyChanges { target: innerDisc; width: Utils.dp(50) }
			},
			State { name: "inactive"
				PropertyChanges { target: innerDisc; width: Utils.dp(25) }
			}
		]

		transitions: [
			Transition { to: "active"
				PropertyAnimation { target: innerDisc; property: "width"; duration: 1500; easing.type: Easing.OutBounce }
			}
		]

		Rectangle {
			id: innerDisc
			anchors.centerIn: parent
			color: tCircle1.state === "active" ? Constants.blue : "white"
			radius: width / 2
			height: width
			border.color: Constants.blue
			border.width: 1
			Text {
				anchors.centerIn: parent
				text: "1"
				font.bold: true
				font.pixelSize: parent.height / 3
				color: tCircle1.state === "active" ? "white" : Constants.blue
			}
		}
	}

	TextCircle {
		id: tCircle2
		anchors.verticalCenter: rec1.verticalCenter
		anchors.horizontalCenter: rec1.horizontalCenter
		text: "2"
	}

	TextCircle {
		id: tCircle3
		anchors.verticalCenter: rec1.verticalCenter
		anchors.horizontalCenter: rec1.right
		text: "3"
	}

	state: "inactive"
	states: [
		State {
			name: "inactive"
			PropertyChanges { target: tCircle1; state: "inactive" }
			PropertyChanges { target: tCircle2; state: "inactive" }
			PropertyChanges { target: tCircle3; state: "inactive" }
			PropertyChanges { target: rec1; anchors.horizontalCenterOffset: _stepWidth }
		},
		State {
			name: "one"
			PropertyChanges { target: tCircle1; state: "active" }
			PropertyChanges { target: tCircle2; state: "inactive" }
			PropertyChanges { target: tCircle3; state: "inactive" }
			PropertyChanges { target: rec1; anchors.horizontalCenterOffset: _stepWidth }
		},
		State {
			name: "two"
			PropertyChanges { target: tCircle1; state: "inactive" }
			PropertyChanges { target: tCircle2; state: "active" }
			PropertyChanges { target: tCircle3; state: "inactive" }
			PropertyChanges { target: rec1; anchors.horizontalCenterOffset: Utils.dp(0) }
		},
		State {
			name: "three"
			PropertyChanges { target: tCircle1; state: "inactive" }
			PropertyChanges { target: tCircle2; state: "inactive" }
			PropertyChanges { target: tCircle3; state: "active" }
			PropertyChanges { target: rec1; anchors.horizontalCenterOffset: -_stepWidth }
		}
	]

	transitions: [
		Transition {
			PropertyAnimation { target: rec1; property: "anchors.horizontalCenterOffset"; duration: 500; easing.type: Easing.InOutCubic }
		}
	]
}
