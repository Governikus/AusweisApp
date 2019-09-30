/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

SequentialAnimation {
	id: expandAnimation
	property real duration: 500
	property var targetContent
	property var targetHeader

	PropertyAction {
		target: expandAnimation.targetContent
		property: "visible"
		value: true
	}

	ParallelAnimation {
		NumberAnimation {
			target: flickable
			property: "contentY"
			easing.type: Easing.InOutQuad
			to: expandAnimation.targetHeader.initY
			duration: expandAnimation.duration
		}

		NumberAnimation {
			target: expandAnimation.targetContent
			property: "height"
			easing.type: Easing.InOutQuad
			from: 0
			to: expandAnimation.targetContent.contentHeight
			duration: expandAnimation.duration
		}
	}
}
