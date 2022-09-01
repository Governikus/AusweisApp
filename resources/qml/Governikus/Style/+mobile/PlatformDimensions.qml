/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.15

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

BrandDimensions {
	readonly property bool isLongLanguage: SettingsModel.language === "uk" || SettingsModel.language === "ru"

	readonly property int button_radius: 6
	readonly property int corner_radius: 10
	readonly property int popup_border: 0
	readonly property int separator_size: 1
	readonly property int list_item_height: 64
	readonly property int scrollbar_padding_vertical: 5
	readonly property int scrollbar_padding_horizontal: 0
	readonly property int scrollbar_width: 5
	readonly property int tutorial_component_spacing: 40
	readonly property int high_contrast_item_border: 0

	readonly property int titlebar_height: 48
	readonly property int titlebar_padding: 8

	readonly property int navigation_bar_height: 49
	readonly property int navigation_bar_padding: 5
	readonly property int navigation_bar_text_padding: 1

	readonly property int progress_bar_height: 32
	readonly property int progress_bar_border: 2

	readonly property real max_text_width: isLongLanguage ? 800 : 700

	readonly property int small_icon_size: 24
	readonly property int icon_size: 48
	readonly property int medium_icon_size: 64
	readonly property int large_icon_size: 96
	readonly property int huge_icon_size: 160
	readonly property int header_icon_size: Constants.is_tablet ? 240 : huge_icon_size

	readonly property int tutorial_title_font_size: 60
	readonly property int tutorial_header_font_size: 26
}
