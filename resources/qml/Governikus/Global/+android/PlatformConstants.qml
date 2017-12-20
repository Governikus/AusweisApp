pragma Singleton

import QtQuick 2.5

import "Utils.js" as Utils

Item {
	readonly property color grey_light: "#bbbbbb"
	readonly property color grey_border: "lightslategrey"
	readonly property color blue_dark: "#324d66"
	readonly property color blue_light: "#659bcd"
	readonly property color primary_text: "#ffffff"
	readonly property color secondary_text: "#444444"
	readonly property color accent_color: "#7879b2"
	readonly property color second_accent_color: "#a3cb7f"
	readonly property int titlebar_font_size: Utils.sp(18)
	readonly property int provider_section_height: Utils.dp(62)
	readonly property int history_section_height: Utils.dp(120)
	readonly property int history_delegate_spacing: Utils.dp(10)
	readonly property color history_delegate_address_color: "#7879b2"
	readonly property int button_height: Utils.dp(36)
	readonly property bool use_history_list_delete_area: false

	readonly property bool is_layout_android: true
	readonly property bool is_layout_ios: false
	readonly property bool is_tablet: false
	readonly property bool leftNavigation: true
	readonly property bool bottomNavigation: false
}
