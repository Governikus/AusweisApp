/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.10
import Governikus.Type.ApplicationModel 1.0

QtObject {
	readonly property int title_font_size: ApplicationModel.scaleFactor * 42
	readonly property int header_font_size: ApplicationModel.scaleFactor * 32
	readonly property int normal_font_size: ApplicationModel.scaleFactor * 26
	readonly property int hint_font_size: ApplicationModel.scaleFactor * 20

	readonly property int corner_radius: ApplicationModel.scaleFactor * 20
	readonly property int corner_radius_popup: corner_radius
	readonly property int button_radius: ApplicationModel.scaleFactor * 15
	readonly property int separator_size: Math.max(ApplicationModel.scaleFactor * 2, 1)
	readonly property real max_text_width: ApplicationModel.scaleFactor * 1000

	readonly property int titlebar_padding: ApplicationModel.scaleFactor * 20

	readonly property int status_icon_large: ApplicationModel.scaleFactor * 350
	readonly property int status_icon_medium: ApplicationModel.scaleFactor * 200
	readonly property int status_icon_small: ApplicationModel.scaleFactor * 100
}
