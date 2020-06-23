/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.10
import Governikus.Global 1.0

BrandDimensions {
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

	readonly property real max_text_width: 500

	readonly property int small_icon_size: 24
	readonly property int icon_size: 48

	readonly property int tutorial_title_font_size: 60
	readonly property int tutorial_header_font_size: 26
}
