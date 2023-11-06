/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick

PlatformConstants {
	readonly property int animation_duration: 250
	readonly property color black: "#000000"
	readonly property color blue: "#164a8c"
	readonly property var flickDeceleration: Constants.is_desktop ? 7500.0 : 1500.0
	readonly property color green: "#327509"
	readonly property color grey: "#8e8e93"
	readonly property real highlightDarkerFactor: 1.3
	readonly property int maximumDeviceNameLength: 33
	readonly property color red: "#9d0100"
	readonly property double scrolling_speed: 7500.0
	readonly property int toolTipDelay: 500
	readonly property color white: "#ffffff"
}
