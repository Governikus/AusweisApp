/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Type

QtObject {
	readonly property real border_width: Math.max(Math.floor(UiPluginModel.scaleFactor * 2), 1)
	readonly property int control_horizontalPadding: UiPluginModel.scaleFactor * 30
	readonly property real control_radius: UiPluginModel.scaleFactor * 26
	readonly property int control_verticalPadding: UiPluginModel.scaleFactor * 10
	readonly property int groupbox_spacing: UiPluginModel.scaleFactor * 20
	readonly property real header_icon_size: UiPluginModel.scaleFactor * 200
	readonly property real huge_icon_size: UiPluginModel.scaleFactor * 125
	readonly property real icon_size: UiPluginModel.scaleFactor * 48
	readonly property bool isLongLanguage: SettingsModel.language === "uk" || SettingsModel.language === "ru"
	readonly property real large_icon_size: UiPluginModel.scaleFactor * 72
	readonly property real lineHeight: UiPluginModel.scaleFactor * 40
	readonly property real lineHeight_button: UiPluginModel.scaleFactor * 27
	readonly property real lineHeight_headline: UiPluginModel.scaleFactor * 47
	readonly property real lineHeight_navigation: UiPluginModel.scaleFactor * 63
	readonly property real lineHeight_subline: UiPluginModel.scaleFactor * 42
	readonly property real lineHeight_tile: UiPluginModel.scaleFactor * 63
	readonly property real lineHeight_title: UiPluginModel.scaleFactor * 70
	readonly property real max_text_width: UiPluginModel.scaleFactor * (isLongLanguage ? 1250 : 1000)
	readonly property real medium_icon_size: UiPluginModel.scaleFactor * 64
	readonly property real min_button_width: UiPluginModel.scaleFactor * 220
	readonly property int pane_border_highlight_width: UiPluginModel.scaleFactor * 4
	readonly property int pane_padding: UiPluginModel.scaleFactor * 30
	readonly property real pane_radius: UiPluginModel.scaleFactor * 34
	readonly property int pane_spacing: UiPluginModel.scaleFactor * 30
	readonly property real popup_border: Math.max(UiPluginModel.scaleFactor * 2, 1)
	readonly property real progress_bar_border: separator_size_large
	readonly property real progress_bar_height: UiPluginModel.scaleFactor * 60
	readonly property real scrollbar_padding_horizontal: UiPluginModel.scaleFactor * 5
	readonly property real scrollbar_padding_vertical: UiPluginModel.scaleFactor * 0
	readonly property real scrollbar_width: UiPluginModel.scaleFactor * 10
	readonly property real separator_size: Math.max(Math.floor(UiPluginModel.scaleFactor * 2), 1)
	readonly property real separator_size_large: Math.max(UiPluginModel.scaleFactor * 4, 1)
	readonly property real small_icon_size: UiPluginModel.scaleFactor * 32
	readonly property real stagedprogressbar_height: UiPluginModel.scaleFactor * 13
	readonly property real stagedprogressbar_spacing: UiPluginModel.scaleFactor * 8
	readonly property real status_icon_medium: UiPluginModel.scaleFactor * 200
	readonly property real status_icon_small: UiPluginModel.scaleFactor * 100
	readonly property int subtext_spacing: Math.max(1, UiPluginModel.scaleFactor * 3)
	readonly property real switch_width: UiPluginModel.scaleFactor * 97
	readonly property real text: UiPluginModel.scaleFactor * 27
	readonly property real textHeadline: UiPluginModel.scaleFactor * 40
	readonly property real textSubline: UiPluginModel.scaleFactor * 30
	readonly property real textTitle: UiPluginModel.scaleFactor * 60
	readonly property real text_navigation: UiPluginModel.scaleFactor * 50
	readonly property int text_spacing: UiPluginModel.scaleFactor * 10
	readonly property real text_tile: UiPluginModel.scaleFactor * 50
	readonly property real titlebar_padding: UiPluginModel.scaleFactor * 20
	readonly property real titlepane_radius: UiPluginModel.scaleFactor * 50
}
