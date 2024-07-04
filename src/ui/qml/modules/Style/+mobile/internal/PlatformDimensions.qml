/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import Governikus.Global
import Governikus.Type

BrandDimensions {
	readonly property int border_width: 1
	readonly property int control_radius: 12
	readonly property real header_icon_size: screenHeight / 7
	readonly property int icon_size: 48
	readonly property bool isLongLanguage: SettingsModel.language === "uk" || SettingsModel.language === "ru"
	readonly property int large_icon_size: 96
	readonly property int lineHeight: 21 * UiPluginModel.fontScaleFactor
	readonly property int lineHeight_button: 15 * UiPluginModel.fontScaleFactor
	readonly property int lineHeight_headline: 34 * UiPluginModel.fontScaleFactor
	readonly property int lineHeight_navigation: 12 * UiPluginModel.fontScaleFactor
	readonly property int lineHeight_subline: 25 * UiPluginModel.fontScaleFactor
	readonly property int lineHeight_tile: 36 * UiPluginModel.fontScaleFactor
	readonly property int lineHeight_title: 36 * UiPluginModel.fontScaleFactor
	readonly property int max_text_width: isLongLanguage ? 800 : 700
	readonly property int medium_icon_size: 64
	readonly property int min_button_width: 104
	readonly property int navigation_bar_icon_size: 20
	readonly property int navigation_bar_min_height: 49
	readonly property int navigation_bar_padding: 10
	readonly property int navigation_bar_text_padding: 5
	readonly property int pageindicator_size: 8
	readonly property int pane_radius: 22
	readonly property int popup_border: 0
	readonly property int progress_bar_border: 2
	readonly property int progress_bar_height: 32
	property int screenHeight: 0
	readonly property int scrollbar_padding_horizontal: 0
	readonly property int scrollbar_padding_vertical: 5
	readonly property int scrollbar_width: 5
	readonly property int separator_size: 1
	readonly property int small_icon_size: 24
	readonly property int status_icon_small: large_icon_size
	readonly property int switch_width: 58
	readonly property int titlebar_padding: 20
	readonly property real workflow_progress_indicator_size: 1.5 * header_icon_size
}
