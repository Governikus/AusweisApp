pragma Singleton

import QtQuick 2.5

import "."

Item {
	readonly property color all_image_background_color: PlatformConstants.all_image_background_color
	readonly property bool use_history_list_delete_area: PlatformConstants.use_history_list_delete_area

	readonly property color background_color: "#dcebf6"
	readonly property color blue: PlatformConstants.blue
	readonly property color blue_dark: PlatformConstants.blue_dark
	readonly property color blue_light: PlatformConstants.blue_light
	readonly property color green: "#a3cb7f"
	readonly property color red: "#cc0000"
	readonly property color grey: "#8e8e93"

	readonly property color primary_text: PlatformConstants.primary_text
	readonly property color secondary_text: PlatformConstants.secondary_text
	readonly property color accent_color: PlatformConstants.accent_color
	readonly property color second_accent_color: PlatformConstants.second_accent_color

	readonly property int header_font_size: Utils.sp(22)
	readonly property int normal_font_size: Utils.sp(16)
	readonly property int label_font_size: Utils.sp(14)
	readonly property int small_font_size: Utils.sp(12)

	readonly property int titlebar_height: Utils.dp(48)
	readonly property int titlebar_padding: Utils.dp(12)
	readonly property int titlebar_font_size: PlatformConstants.titlebar_font_size

	readonly property int menubar_width: Utils.dp(60)

	readonly property int searchbar_height: Utils.dp(48)

	readonly property int provider_section_height: PlatformConstants.provider_section_height

	readonly property int history_section_height: PlatformConstants.history_section_height
	readonly property int history_delegate_spacing: PlatformConstants.history_delegate_spacing
	readonly property color history_delegate_address_color: PlatformConstants.history_delegate_address_color

	readonly property int button_height: PlatformConstants.button_height

	readonly property int tabbar_height: Utils.dp(48)

	readonly property int component_spacing: Utils.dp(20)
	readonly property int pane_padding: Utils.dp(20)
	readonly property int pane_spacing: Utils.dp(20)
}
