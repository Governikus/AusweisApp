pragma Singleton

import QtQuick 2.5
import QtQuick.Window 2.2

import "Utils.js" as Utils
import "." as Gov


Item {
	readonly property bool use_history_list_delete_area: Gov.PlatformConstants.use_history_list_delete_area

	readonly property color background_color: "#dcebf6"
	readonly property color blue: "#659bcd"
	readonly property color green: "#a3cb7f"
	readonly property color red: "#cc0000"
	readonly property color grey: "#8e8e93"
	readonly property color lightgrey: "#d5d5dc"
	readonly property color white: "#ffffff"
	readonly property color black: "#000000"

	readonly property color primary_text: Gov.PlatformConstants.primary_text
	readonly property color secondary_text: Gov.PlatformConstants.secondary_text
	readonly property color accent_color: Gov.PlatformConstants.accent_color
	readonly property color second_accent_color: Gov.PlatformConstants.second_accent_color

	readonly property int header_font_size: thresholdReduce(22)
	readonly property int normal_font_size: thresholdReduce(16)
	readonly property int label_font_size: Utils.sp(14)
	readonly property int small_font_size: Utils.sp(12)

	readonly property int titlebar_height: Utils.dp(48)
	readonly property int titlebar_padding: Utils.dp(12)
	readonly property int titlebar_spacing: Utils.dp(18)
	readonly property int titlebar_font_size: Gov.PlatformConstants.titlebar_font_size

	readonly property int menubar_width: Utils.dp(60)

	readonly property int searchbar_height: Utils.dp(48)

	readonly property int provider_section_height: Gov.PlatformConstants.provider_section_height

	readonly property int history_section_height: Gov.PlatformConstants.history_section_height
	readonly property int history_delegate_spacing: Gov.PlatformConstants.history_delegate_spacing
	readonly property color history_delegate_address_color: Gov.PlatformConstants.history_delegate_address_color

	readonly property int button_height: Gov.PlatformConstants.button_height

	readonly property int tabbar_height: Utils.dp(48)

	readonly property int component_spacing: Utils.dp(20)
	readonly property int pane_padding: Utils.dp(20)
	readonly property int pane_spacing: Utils.dp(20)
	readonly property int groupbox_spacing: Utils.dp(10)

	function thresholdReduce(value) {
		var w = Screen.width
		if (w > 415) {
			return Utils.sp(value)
		}
		return Utils.sp(value * w / 415)
	}
}
