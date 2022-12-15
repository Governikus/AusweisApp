/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15

SequentialAnimation {
	id: collapseAnimation

	property real duration: 500
	property var targetContent

	ParallelAnimation {
		NumberAnimation {
			duration: collapseAnimation.duration
			easing.type: Easing.InOutQuad
			property: "height"
			target: collapseAnimation.targetContent
			to: 0
		}
	}
	PropertyAction {
		property: "visible"
		target: collapseAnimation.targetContent
		value: false
	}
}
