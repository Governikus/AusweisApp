/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick 2.12

PlatformConstants {
	readonly property color blue: "#4d7aa4"
	readonly property color green: "#a3cb7f"
	readonly property color darkgreen: "#017f01"
	readonly property color red: "#cc0000"
	readonly property color grey: "#8e8e93"
	readonly property color lightgrey: "#d5d5dc"
	readonly property color white: "#ffffff"
	readonly property color black: "#000000"

	readonly property double scrolling_speed: 7500.0
	readonly property var flickDeceleration: Constants.is_desktop ? 7500.0 : 1500.0

	readonly property int animation_duration: 250
	readonly property int toolTipDelay: 500

	readonly property real highlightDarkerFactor: 1.3

	readonly property int maximumDeviceNameLength: 33
}
