pragma Singleton

import QtQuick 2.5

import "Utils.js" as Utils

Item {
	readonly property color grey_light: "#8e8e93"
	readonly property color grey_border: "lightslategrey"
	readonly property color blue_dark: "#0076ff"
	readonly property color blue_light: "#54c7fc"
	readonly property color primary_text: "#ffffff"
	readonly property color secondary_text: "#000000"
	readonly property color accent_color: "#000000"
	readonly property color second_accent_color: "#000000"
	readonly property int titlebar_font_size: Utils.sp(16)
	readonly property int provider_section_height: Utils.dp(50)
	readonly property int history_section_height: Utils.dp(85)
	readonly property int history_delegate_spacing: 0
	readonly property color history_delegate_address_color: "#00878F"
	readonly property int button_height: Utils.dp(40)
	readonly property bool use_history_list_delete_area: true

	readonly property bool is_layout_android: false
	readonly property bool is_layout_ios: true
	readonly property bool is_tablet: false
	readonly property bool leftNavigation: false
	readonly property bool bottomNavigation: true
}
