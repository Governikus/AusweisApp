/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

SequentialAnimation {
	id: collapseAnimation
	property real duration: 500
	property var targetContent

	ParallelAnimation {
		NumberAnimation {
			target: collapseAnimation.targetContent
			property: "height"
			easing.type: Easing.InOutQuad
			to: 0
			duration: collapseAnimation.duration
		}
	}

	PropertyAction {
		target: collapseAnimation.targetContent
		property: "visible"
		value: false
	}
}
