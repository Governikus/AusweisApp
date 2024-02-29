/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

Transition {
	property alias target: animation.target
	property alias targets: animation.targets

	to: "pressed"

	ColorAnimation {
		id: animation

		duration: Constants.eased_in_pressed_duration
		easing.type: Easing.InCubic
	}
}
