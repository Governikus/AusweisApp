/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Type.SettingsModel

QtObject {
	readonly property real border_width: Math.max(Math.floor(plugin.scaleFactor * 2), 1)
	readonly property real control_radius: plugin.scaleFactor * 26
	readonly property real header_icon_size: plugin.scaleFactor * 200
	readonly property real huge_icon_size: plugin.scaleFactor * 125
	readonly property real icon_size: plugin.scaleFactor * 48
	readonly property bool isLongLanguage: SettingsModel.language === "uk" || SettingsModel.language === "ru"
	readonly property real large_icon_size: plugin.scaleFactor * 72
	readonly property real lineHeight: plugin.scaleFactor * 40
	readonly property real lineHeight_button: plugin.scaleFactor * 27
	readonly property real lineHeight_headline: plugin.scaleFactor * 47
	readonly property real lineHeight_navigation: plugin.scaleFactor * 63
	readonly property real lineHeight_subline: plugin.scaleFactor * 42
	readonly property real lineHeight_tile: plugin.scaleFactor * 63
	readonly property real lineHeight_title: plugin.scaleFactor * 70
	readonly property real list_item_height: plugin.scaleFactor * 64
	readonly property real max_text_width: plugin.scaleFactor * (isLongLanguage ? 1250 : 1000)
	readonly property real medium_icon_size: plugin.scaleFactor * 64
	readonly property real min_button_width: plugin.scaleFactor * 220
	readonly property real pane_radius: plugin.scaleFactor * 34
	readonly property real popup_border: Math.max(plugin.scaleFactor * 2, 1)
	readonly property real progress_bar_border: separator_size_large
	readonly property real progress_bar_height: plugin.scaleFactor * 60
	readonly property real scrollbar_padding_horizontal: plugin.scaleFactor * 5
	readonly property real scrollbar_padding_vertical: plugin.scaleFactor * 0
	readonly property real scrollbar_width: plugin.scaleFactor * 10
	readonly property real separator_size: Math.max(plugin.scaleFactor * 2, 1)
	readonly property real separator_size_large: Math.max(plugin.scaleFactor * 4, 1)
	readonly property real small_icon_size: plugin.scaleFactor * 32
	readonly property real status_icon_medium: plugin.scaleFactor * 200
	readonly property real status_icon_small: plugin.scaleFactor * 100
	readonly property real switch_width: plugin.scaleFactor * 97
	readonly property real text: plugin.scaleFactor * 27
	readonly property real text_headline: plugin.scaleFactor * 40
	readonly property real text_navigation: plugin.scaleFactor * 50
	readonly property real text_subline: plugin.scaleFactor * 30
	readonly property real text_tile: plugin.scaleFactor * 50
	readonly property real text_title: plugin.scaleFactor * 60
	readonly property real titlebar_padding: plugin.scaleFactor * 20
	readonly property real titlepane_radius: plugin.scaleFactor * 50
}
