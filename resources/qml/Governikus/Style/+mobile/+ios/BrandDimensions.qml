/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Window
import Governikus.Global

QtObject {
	readonly property int navigation_bar_bottom_padding: 1
	// If this font size changes, it has to be changed in UIPlugInQml, too.
	readonly property real text: scaleText(15) * plugin.fontScaleFactor
	readonly property real text_headline: scaleText(26) * plugin.fontScaleFactor
	readonly property real text_navigation: scaleText(12) * plugin.fontScaleFactor
	readonly property real text_subline: scaleText(18) * plugin.fontScaleFactor
	readonly property real text_tile: scaleText(30) * plugin.fontScaleFactor
	readonly property real text_title: scaleText(30) * plugin.fontScaleFactor

	// Scale the text on small devices like the iPhone SE
	function scaleText(value) {
		let w = Screen.width;
		if (w > 415) {
			return value;
		}
		return value * w / 415;
	}
}
