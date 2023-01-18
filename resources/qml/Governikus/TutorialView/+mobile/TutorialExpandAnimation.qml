/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

SequentialAnimation {
	id: expandAnimation

	property real duration: 500
	property var targetContent
	property var targetHeader

	PropertyAction {
		property: "visible"
		target: expandAnimation.targetContent
		value: true
	}
	ParallelAnimation {
		NumberAnimation {
			duration: expandAnimation.duration
			easing.type: Easing.InOutQuad
			property: "contentY"
			target: flickable
			to: expandAnimation.targetHeader.initY
		}
		NumberAnimation {
			duration: expandAnimation.duration
			easing.type: Easing.InOutQuad
			from: 0
			property: "height"
			target: expandAnimation.targetContent
			to: expandAnimation.targetContent.contentHeight
		}
	}
}
