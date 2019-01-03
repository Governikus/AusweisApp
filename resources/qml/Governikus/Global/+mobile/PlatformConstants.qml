import QtQuick 2.10
import QtQuick.Window 2.2

import "Utils.js" as Utils

BrandConstants {
	readonly property color blue: "#659bcd"
	readonly property color background_color: "#dcebf6"
	readonly property color grey_border: "lightslategrey"

	readonly property color tutorial_orange: "#f9a501"
	readonly property color tutorial_green: "#73d7b3"
	readonly property color tutorial_blue: "#659bcd"
	readonly property color tutorial_red: "#fb7a59"
	readonly property color tutorial_very_light_grey: "#f2f2f2"

	readonly property int header_font_size: thresholdReduce(22)
	readonly property int pane_title_font_size: header_font_size
	readonly property int normal_font_size: thresholdReduce(16)
	readonly property int label_font_size: Utils.dp(14)
	readonly property int small_font_size: Utils.dp(12)

	readonly property int tutorial_component_spacing: Utils.dp(40)
	readonly property int tutorial_header_font_size: Utils.dp(60)
	readonly property int tutorial_content_header_h1_font_size: Utils.dp(26)
	readonly property int tutorial_content_header_h2_font_size: Utils.dp(20)
	readonly property int tutorial_content_font_size: Utils.dp(16)

	readonly property int menubar_width: Utils.dp(60)

	readonly property int titlebar_height: Utils.dp(48)
	readonly property int titlebar_padding: Utils.dp(12)
	readonly property int titlebar_spacing: Utils.dp(18)

	readonly property int component_spacing: Utils.dp(20)
	readonly property int pane_padding: Utils.dp(20)
	readonly property int pane_spacing: Utils.dp(20)
	readonly property int groupbox_spacing: Utils.dp(10)

	readonly property bool is_desktop: false

	function thresholdReduce(value) {
		var w = Screen.width
		if (w > 415) {
			return Utils.dp(value)
		}
		return Utils.dp(value * w / 415)
	}
}
