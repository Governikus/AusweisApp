/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property Component sourceComponent
	readonly property var currentSectionPage: stack.currentItem

	property alias stackView: stack

	StackView {
		id: stack

		anchors.fill: parent

		initialItem: baseItem.sourceComponent
	}

	MouseArea {
		id: iosBackGestureMouseArea

		readonly property real minSwipeDistance: parent.width * 0.2
		readonly property real minVelocity: 10
		readonly property real touchStartAreaWidth: 10
		property real startPosX: 0.0
		property real previousPosX: 0.0
		property real velocity: 0.0

		anchors.fill: parent

		enabled: Constants.is_layout_ios
		preventStealing: true

		onPressed: {
			if (mouse.x < touchStartAreaWidth && currentSectionPage.navigationAction.state === "back") {
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
				currentSectionPage.navigationAction.clicked()
			}
		}
	}

	onVisibleChanged: {
		if (currentSectionPage) {
			currentSectionPage.activated()
		}
	}

	onCurrentSectionPageChanged: {
		if (currentSectionPage) {
			currentSectionPage.activated()
		}
	}
}
