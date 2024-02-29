/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick

PlatformConstants {
	readonly property int animation_duration: 250
	readonly property color black: "#000000"
	readonly property color blue: "#164a8c"
	readonly property int eased_in_pressed_duration: 150
	readonly property var flickDeceleration: Constants.is_desktop ? 7500.0 : 1500.0
	readonly property color green: "#327509"
	readonly property real highlightDarkerFactor: 1.3
	readonly property int maximumDeviceNameLength: 33
	readonly property double scrolling_speed: 7500.0
	readonly property int toolTipDelay: 500
	readonly property color white: "#ffffff"
}
