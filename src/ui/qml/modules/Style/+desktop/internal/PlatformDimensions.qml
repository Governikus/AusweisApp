/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Type

QtObject {
	readonly property real border_width: Math.max(Math.floor(UiPluginModel.scaleFactor * 1.2), 1)
	readonly property int control_horizontalPadding: UiPluginModel.scaleFactor * 18
	readonly property real control_radius: UiPluginModel.scaleFactor * 15.6
	readonly property int control_verticalPadding: UiPluginModel.scaleFactor * 6
	readonly property int groupbox_spacing: UiPluginModel.scaleFactor * 12
	readonly property real header_icon_size: UiPluginModel.scaleFactor * 120
	readonly property real huge_icon_size: UiPluginModel.scaleFactor * 75
	readonly property real icon_size: UiPluginModel.scaleFactor * 28.8
	readonly property bool isLongLanguage: SettingsModel.language === "uk" || SettingsModel.language === "ru"
	readonly property real large_icon_size: UiPluginModel.scaleFactor * 43.2
	readonly property real lineHeight: UiPluginModel.scaleFactor * 24
	readonly property real lineHeight_button: UiPluginModel.scaleFactor * 16.2
	readonly property real lineHeight_headline: UiPluginModel.scaleFactor * 28.2
	readonly property real lineHeight_navigation: UiPluginModel.scaleFactor * 37.8
	readonly property real lineHeight_subline: UiPluginModel.scaleFactor * 25.2
	readonly property real lineHeight_tile: UiPluginModel.scaleFactor * 37.8
	readonly property real lineHeight_title: UiPluginModel.scaleFactor * 42
	readonly property real max_text_width: UiPluginModel.scaleFactor * (isLongLanguage ? 750 : 600)
	readonly property real medium_icon_size: UiPluginModel.scaleFactor * 38.4
	readonly property real min_button_width: UiPluginModel.scaleFactor * 132
	readonly property int pane_border_highlight_width: UiPluginModel.scaleFactor * 2.4
	readonly property int pane_padding: UiPluginModel.scaleFactor * 18
	readonly property real pane_radius: UiPluginModel.scaleFactor * 20.4
	readonly property int pane_spacing: UiPluginModel.scaleFactor * 18
	readonly property real popup_border: Math.max(UiPluginModel.scaleFactor * 1.2, 1)
	readonly property real progress_bar_border: separator_size_large
	readonly property real progress_bar_height: UiPluginModel.scaleFactor * 36
	readonly property real scrollbar_padding_horizontal: UiPluginModel.scaleFactor * 3
	readonly property real scrollbar_padding_vertical: UiPluginModel.scaleFactor * 0
	readonly property real scrollbar_width: UiPluginModel.scaleFactor * 6
	readonly property real separator_size: Math.max(Math.floor(UiPluginModel.scaleFactor * 1.2, 1))
	readonly property real separator_size_large: Math.max(UiPluginModel.scaleFactor * 2.4, 1)
	readonly property real small_icon_size: UiPluginModel.scaleFactor * 19.2
	readonly property real stagedprogressbar_height: UiPluginModel.scaleFactor * 7.8
	readonly property real stagedprogressbar_spacing: UiPluginModel.scaleFactor * 4.8
	readonly property real status_icon_medium: UiPluginModel.scaleFactor * 120
	readonly property real status_icon_small: UiPluginModel.scaleFactor * 60
	readonly property int subtext_spacing: Math.max(1, UiPluginModel.scaleFactor * 1.8)
	readonly property real switch_width: UiPluginModel.scaleFactor * 58.2
	readonly property real text: UiPluginModel.scaleFactor * 16.2
	readonly property real textHeadline: UiPluginModel.scaleFactor * 24
	readonly property real textSubline: UiPluginModel.scaleFactor * 18
	readonly property real textTitle: UiPluginModel.scaleFactor * 36
	readonly property real text_navigation: UiPluginModel.scaleFactor * 30
	readonly property int text_spacing: UiPluginModel.scaleFactor * 6
	readonly property real text_tile: UiPluginModel.scaleFactor * 30
	readonly property real titlebar_padding: UiPluginModel.scaleFactor * 12
	readonly property real titlepane_radius: UiPluginModel.scaleFactor * 30
}
