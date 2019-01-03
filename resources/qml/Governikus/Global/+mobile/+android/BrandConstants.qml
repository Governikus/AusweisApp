import QtQuick 2.10

import "Utils.js" as Utils

DeviceConstants {
	readonly property color grey_light: "#bbbbbb"
	readonly property color blue_dark: "#324d66"
	readonly property color blue_light: "#659bcd"
	readonly property color accent_color: "#7879b2"
	readonly property color primary_text: "#ffffff"
	readonly property color secondary_text: "#4a4a4a"
	readonly property int titlebar_font_size: Utils.dp(18)
	readonly property int provider_section_height: Utils.dp(62)
	readonly property int history_delegate_spacing: Utils.dp(10)
	readonly property color history_delegate_address_color: "#7879b2"
	readonly property int button_height: Utils.dp(36)

	readonly property bool is_layout_android: true
	readonly property bool is_layout_ios: false
	readonly property bool leftNavigation: true
	readonly property bool bottomNavigation: false
}
