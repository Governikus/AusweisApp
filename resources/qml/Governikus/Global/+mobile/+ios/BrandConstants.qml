import QtQuick 2.10

import "Utils.js" as Utils

DeviceConstants {
	readonly property color grey_light: "#8e8e93"
	readonly property color blue_dark: "#0076ff"
	readonly property color blue_light: "#54c7fc"
	readonly property color accent_color: "#000000"
	readonly property color secondary_text: "#000000"
	readonly property int titlebar_font_size: Utils.dp(16)
	readonly property int provider_section_height: Utils.dp(50)
	readonly property int history_delegate_spacing: 0
	readonly property color history_delegate_address_color: "#00878F"
	readonly property int button_height: Utils.dp(40)

	readonly property bool is_layout_android: false
	readonly property bool is_layout_ios: true
	readonly property bool leftNavigation: false
	readonly property bool bottomNavigation: true

	readonly property int searchbar_height: Utils.dp(48)
	readonly property int tabbar_height: Utils.dp(48)
}
