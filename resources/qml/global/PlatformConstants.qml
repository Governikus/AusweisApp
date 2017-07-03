pragma Singleton

import QtQuick 2.5

import "Utils.js" as Utils

Item {
	// iOS-specific constants, according to the iOS Human Interface Guidelines
	readonly property color ios_blue_light: "#54c7fc"
	readonly property color ios_yellow: "#ffcd00"
	readonly property color ios_orange: "#ff9600"
	readonly property color ios_purple: "#ff2851"
	readonly property color ios_blue: "#0076ff"
	readonly property color ios_green: "#44db5e"
	readonly property color ios_red: "#ff3824"
	readonly property color ios_grey: "#8e8e93"

	readonly property color all_image_background_color: "transparent"
	readonly property color blue: "#659bcd"
	readonly property color blue_dark: ios_blue
	readonly property color blue_light: ios_blue_light
	readonly property color primary_text: "#ffffff"
	readonly property color secondary_text: "#000000"
	readonly property color accent_color: "#000000"
	readonly property color second_accent_color: "#000000"
	readonly property int titlebar_font_size: Utils.sp(16)
	readonly property int provider_section_height: Utils.dp(50)
	readonly property int history_section_height: Utils.dp(60)
	readonly property int history_delegate_spacing: 0
	readonly property color history_delegate_address_color: "#00878F"
	readonly property int button_height: Utils.dp(40)
	readonly property bool use_history_list_delete_area: true

	readonly property bool is_tablet: false
	readonly property bool leftNavigation: false
	readonly property bool bottomNavigation: true
}
