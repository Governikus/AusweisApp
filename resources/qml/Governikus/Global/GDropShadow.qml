/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Effects

MultiEffect {
	property real verticalOffset: 7

	shadowEnabled: true
	shadowOpacity: 0.15
	shadowScale: 1.025

	// Timer is necessary because shadowVerticalOffset does not get updated
	// after initialization. Only an external update after the whole UI has
	// been loaded leads to the expected result, else the drop shadow will
	// be drawn too low.
	Timer {
		interval: 1
		running: true

		onTriggered: shadowVerticalOffset = Qt.binding(function () {
				return verticalOffset;
			})
	}
}
