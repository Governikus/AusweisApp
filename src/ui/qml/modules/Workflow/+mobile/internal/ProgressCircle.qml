/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: baseItem

	readonly property int stepWidth: 60

	implicitHeight: 70
	state: "inactive"

	states: [
		State {
			name: "inactive"

			PropertyChanges {
				rec1.anchors.leftMargin: 0
				tCircle1.state: "inactive"
				tCircle2.state: "inactive"
			}
		},
		State {
			name: "one"

			PropertyChanges {
				rec1.anchors.leftMargin: 0
				tCircle1.state: "active"
				tCircle2.state: "inactive"
			}
		},
		State {
			name: "two"

			PropertyChanges {
				rec1.anchors.leftMargin: -stepWidth
				tCircle1.state: "inactive"
				tCircle2.state: "active"
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
		width: stepWidth
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
					innerDisc.width: 50
				}
			},
			State {
				name: "inactive"

				PropertyChanges {
					innerDisc.width: 25
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
			color: tCircle1.state === "active" ? Style.color.control.background.basic : Constants.white
			height: width
			radius: width / 2

			GText {
				Accessible.ignored: true
				anchors.centerIn: parent
				color: tCircle1.state === "active" ? Constants.white : Style.color.control.background.basic
				font.bold: true
				font.pixelSize: parent.height / 3
				text: "1"
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
