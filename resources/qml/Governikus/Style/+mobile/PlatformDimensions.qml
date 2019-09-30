/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.10
import Governikus.Global 1.0

BrandDimensions {
	readonly property int button_radius: 3
	readonly property int separator_size: 1
	readonly property int list_item_height: 64
	readonly property int scrollbar_padding: 5
	readonly property int tutorial_component_spacing: 40

	readonly property int titlebar_height: 48
	readonly property int titlebar_padding: 8

	readonly property real max_text_width: 500

	readonly property int small_icon_size: 24
	readonly property int icon_size: 48

	readonly property int tutorial_title_font_size: 60
	readonly property int tutorial_header_font_size: 26
}
