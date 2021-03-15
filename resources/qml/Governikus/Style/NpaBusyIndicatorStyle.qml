/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQml 2.2
import QtGraphicalEffects 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.Random 1.0

Item {
	property real factor: 1.1

	id: busyIndicator
	anchors.centerIn: parent

	state: parent.running ? "running" : "notrunning"
	states: [
		State {
			name: "running"
			PropertyChanges { target: timer; running: true }
			PropertyChanges { target: busyIndicator; rotation: 0 }
			PropertyChanges { target: green; rotation: 0 }
			PropertyChanges { target: blue; rotation: 0 }
			PropertyChanges { target: rect; opacity: 1 }
		},
		State {
			name: "notrunning"
			PropertyChanges { target: timer; running: false }
			PropertyChanges { target: rect; opacity: 0 }
		}
	]

	transitions: Transition {
		NumberAnimation { properties: "opacity"}
	}

	Behavior on rotation {
		NumberAnimation { duration: timer.interval; easing.type: Easing.Linear }
	}

	Rectangle {
		id: rect
		anchors.centerIn: parent
		height: parent.height * factor
		width: height
		radius: width / 2
		color: Style.color.background
		opacity: 0

		Behavior on opacity {
			NumberAnimation { duration: 200 }
		}
	}

	Timer {
		id: timer
		interval: 1000; repeat: true
		onTriggered: {
			green.rotation = green.rotation + Random.randomInt(0, 134)
			blue.rotation = blue.rotation + Random.randomInt(0, 194)
			busyIndicator.rotation = busyIndicator.rotation + 100
		}
	}

	ConicalGradient {
		id: green
		anchors.fill: rect
		source: rect
		angle: 0.0
		cached: true
		opacity: rect.opacity
		gradient: Gradient
		{
			GradientStop {color: Constants.green; position: 0.0}
			GradientStop {color: Constants.green; position: 0.50}
			GradientStop {color: Style.color.transparent; position: 0.5000000000000001}
			GradientStop {color: Style.color.transparent; position: 1.0}
		}
		Behavior on rotation {
			NumberAnimation { duration: timer.interval; easing.type: Easing.InOutQuad }
		}
	}
	ConicalGradient {
		id: blue
		anchors.fill: rect
		source: rect
		angle: 0.0
		cached: true
		opacity: rect.opacity
		gradient: Gradient
		{
			GradientStop {color: Style.color.transparent; position: 0.0}
			GradientStop {color: Style.color.transparent; position: 0.50}
			GradientStop {color: rect.color; position: 0.5000000000000001}
			GradientStop {color: Constants.blue; position: 1.0}
		}
		Behavior on rotation {
			NumberAnimation { duration: timer.interval; easing.type: Easing.InOutQuad }
		}
	}
}
