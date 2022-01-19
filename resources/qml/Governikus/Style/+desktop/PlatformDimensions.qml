/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.12
import Governikus.Type.ApplicationModel 1.0

QtObject {
	readonly property int title_font_size: ApplicationModel.scaleFactor * 42
	readonly property int header_font_size: ApplicationModel.scaleFactor * 32
	readonly property int normal_font_size: ApplicationModel.scaleFactor * 26
	readonly property int hint_font_size: ApplicationModel.scaleFactor * 22
	readonly property int navigation_font_size: header_font_size

	readonly property int corner_radius: ApplicationModel.scaleFactor * 20
	readonly property int corner_radius_popup: corner_radius
	readonly property int button_radius: ApplicationModel.scaleFactor * 15
	readonly property int popup_border: Math.max(ApplicationModel.scaleFactor * 2, 1)
	readonly property int separator_size: Math.max(ApplicationModel.scaleFactor * 2, 1)
	readonly property int separator_size_large: Math.max(ApplicationModel.scaleFactor * 4, 1)
	readonly property int tabbed_pane_separator_size: separator_size_large
	readonly property real max_text_width: ApplicationModel.scaleFactor * 1000
	readonly property int list_item_height: ApplicationModel.scaleFactor * 64
	readonly property int high_contrast_item_border: ApplicationModel.scaleFactor * 0

	readonly property int titlebar_padding: ApplicationModel.scaleFactor * 20

	readonly property int scrollbar_padding_vertical: ApplicationModel.scaleFactor * 0
	readonly property int scrollbar_padding_horizontal: ApplicationModel.scaleFactor * 5
	readonly property int scrollbar_width: ApplicationModel.scaleFactor * 10

	readonly property int status_icon_large: ApplicationModel.scaleFactor * 350
	readonly property int status_icon_medium: ApplicationModel.scaleFactor * 200
	readonly property int status_icon_small: ApplicationModel.scaleFactor * 100

	readonly property int icon_size: ApplicationModel.scaleFactor * 48
	readonly property int medium_icon_size: ApplicationModel.scaleFactor * 64
	readonly property int large_icon_size: ApplicationModel.scaleFactor * 72
	readonly property int huge_icon_size: ApplicationModel.scaleFactor * 200
}
