pragma Singleton

import QtQuick 2.5

import "Utils.js" as Utils

Item {
	// Android-specific constants
	readonly property color blue_primary: "#659bcd"
	readonly property color blue_primary_dark: "#324d66"
	readonly property color lila_accent_color: "#7879b2"
	readonly property color green_second_accent_color: "#a3cb7f"
	readonly property color white_primary_text: "#ffffff"
	readonly property color dark_grey_secondary_text: "#444444"
	readonly property color grey_light: "#bbbbbb"
	readonly property color grey_border: "lightslategrey"

	readonly property color all_image_background_color: "#dcebf6"
	readonly property color blue: blue_primary
	readonly property color blue_dark: blue_primary_dark
	readonly property color blue_light: blue_primary
	readonly property color primary_text: white_primary_text
	readonly property color secondary_text: dark_grey_secondary_text
	readonly property color accent_color: lila_accent_color
	readonly property color second_accent_color: green_second_accent_color
	readonly property int titlebar_font_size: Utils.sp(18)
	readonly property int provider_section_height: Utils.dp(62)
	readonly property int history_section_height: Utils.dp(120)
	readonly property int history_delegate_spacing: Utils.dp(10)
	readonly property color history_delegate_address_color: lila_accent_color
	readonly property int button_height: Utils.dp(36)
	readonly property bool use_history_list_delete_area: false

	readonly property bool is_tablet: false
	readonly property bool leftNavigation: true
	readonly property bool bottomNavigation: false
}
