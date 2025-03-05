/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

Transition {
	property alias target: animation.target
	property alias targets: animation.targets

	to: "pressed"

	ColorAnimation {
		id: animation

		duration: 150
		easing.type: Easing.InCubic
	}
}
