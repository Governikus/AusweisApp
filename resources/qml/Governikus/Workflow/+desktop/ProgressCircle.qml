/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Item {
	QtObject {
		id: d
		readonly property int stepWidth: ApplicationModel.scaleFactor * 250
	}

	height: selector.height
	width: d.stepWidth * 4 + height

	state: "1"
	states: [
		State {
			name: "1"
			PropertyChanges { target: circle1; enabled: true }
			PropertyChanges { target: circle2; enabled: false }
			PropertyChanges { target: circle3; enabled: false }
			PropertyChanges { target: line; anchors.leftMargin: 0 }
		},
		State {
			name: "2"
			PropertyChanges { target: circle1; enabled: false }
			PropertyChanges { target: circle2; enabled: true }
			PropertyChanges { target: circle3; enabled: false }
			PropertyChanges { target: line; anchors.leftMargin: -d.stepWidth }
		},
		State {
			name: "3"
			PropertyChanges { target: circle1; enabled: false }
			PropertyChanges { target: circle2; enabled: false }
			PropertyChanges { target: circle3; enabled: true }
			PropertyChanges { target: line; anchors.leftMargin: 2 * -d.stepWidth }
		}
	]
	transitions: [
		Transition {
			PropertyAnimation { target: line; property: "anchors.leftMargin"; duration: 500; easing.type: Easing.InOutCubic }
			SequentialAnimation {
				PauseAnimation { duration: 200 }
				PropertyAction { target: circle1; property: "enabled" }
				PropertyAction { target: circle2; property: "enabled" }
				PropertyAction { target: circle3; property: "enabled" }
			}
		}
	]

	Rectangle {
		id: line
		anchors.left: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		width: d.stepWidth * 2
		height: ApplicationModel.scaleFactor * 8
		color: Style.color.border
	}

	TextCircle {
		id: circle1
		anchors.verticalCenter: line.verticalCenter
		anchors.horizontalCenter: line.left
		text: "1"
	}

	TextCircle {
		id: circle2
		anchors.verticalCenter: line.verticalCenter
		anchors.horizontalCenter: line.horizontalCenter
		text: "2"
	}

	TextCircle {
		id: circle3
		anchors.verticalCenter: line.verticalCenter
		anchors.horizontalCenter: line.right
		text: "3"
	}

	Rectangle {
		id: selector
		height: circle1.height + ApplicationModel.scaleFactor * 40
		width: height
		anchors.centerIn: parent
		color: Style.color.transparent
		radius: height / 2
		border.color: Style.color.primary_text_inverse
		border.width: ApplicationModel.scaleFactor * 6
	}
}
