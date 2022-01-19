/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem
	height: 70
	readonly property int _stepWidth: 60

	Rectangle {
		id: rec1
		anchors.left: baseItem.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		width: _stepWidth
		height: 2
		color: Constants.blue
	}

	Rectangle {
		id: tCircle1
		anchors.verticalCenter: rec1.verticalCenter
		anchors.horizontalCenter: rec1.left
		width: state === "active" ? 70 : 25 + border.width
		height: width
		radius: width / 2
		color: Style.color.background
		border.color: Constants.white
		border.width: state === "active" ? 2 : 0

		state: "inactive"
		states: [
			State { name: "active"
				PropertyChanges { target: innerDisc; width: 50 }
			},
			State { name: "inactive"
				PropertyChanges { target: innerDisc; width: 25 }
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
			color: tCircle1.state === "active" ? Style.color.accent : Constants.white
			radius: width / 2
			height: width
			border.color: Constants.blue
			border.width: 1
			Text {
				anchors.centerIn: parent
				text: "1"
				font.bold: true
				font.pixelSize: parent.height / 3
				color: tCircle1.state === "active" ? Constants.white : Style.color.accent
			}
		}
	}

	TextCircle {
		id: tCircle2
		anchors.verticalCenter: rec1.verticalCenter
		anchors.horizontalCenter: rec1.right
		text: "2"
	}

	state: "inactive"
	states: [
		State {
			name: "inactive"
			PropertyChanges { target: tCircle1; state: "inactive" }
			PropertyChanges { target: tCircle2; state: "inactive" }
			PropertyChanges { target: rec1; anchors.leftMargin: 0 }
		},
		State {
			name: "one"
			PropertyChanges { target: tCircle1; state: "active" }
			PropertyChanges { target: tCircle2; state: "inactive" }
			PropertyChanges { target: rec1; anchors.leftMargin: 0 }
		},
		State {
			name: "two"
			PropertyChanges { target: tCircle1; state: "inactive" }
			PropertyChanges { target: tCircle2; state: "active" }
			PropertyChanges { target: rec1; anchors.leftMargin: -_stepWidth }
		}
	]

	transitions: [
		Transition {
			to: "two"
			PropertyAnimation { target: rec1; property: "anchors.leftMargin"; duration: 500; easing.type: Easing.InOutCubic }
		}
	]
}
