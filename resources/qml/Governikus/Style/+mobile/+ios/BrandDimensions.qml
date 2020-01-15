/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Window 2.2
import Governikus.Global 1.0

QtObject {
	readonly property int corner_radius_popup: corner_radius

	readonly property int tabbar_height: 49
	readonly property int searchbar_height: 48

	readonly property int title_font_size: scaleText(22)
	readonly property int header_font_size: scaleText(20)
	readonly property int normal_font_size: scaleText(16)
	readonly property int hint_font_size: scaleText(14)

	// Scale the text on small devices like the iPhone SE
	function scaleText(value) {
		var w = Screen.width
		if (w > 415) {
			return value
		}
		return value * w / 415
	}
}
