/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

MouseArea {
	id: root

	signal backGestureTriggered()

	readonly property real minSwipeDistance: parent.width * 0.2
	readonly property real minVelocity: 10
	readonly property real touchStartAreaWidth: 10
	property real startPosX: 0.0
	property real previousPosX: 0.0
	property real velocity: 0.0

	enabled: visible
	preventStealing: enabled

	onPressed: mouse => {
		if (mouse.x < touchStartAreaWidth) {
			mouse.accepted = true
			startPosX = mouse.x
			previousPosX = startPosX
			velocity = 0.0
		} else {
			mouse.accepted = false
		}
	}

	onPositionChanged: {
		let currentVelocity = mouse.x - previousPosX
		velocity = (velocity + currentVelocity) / 2.0
		previousPosX = mouse.x
	}

	onReleased: {
		let swipeDistance = mouse.x - startPosX
		if (swipeDistance > minSwipeDistance && velocity > minVelocity) {
			backGestureTriggered()
		}
	}
}
