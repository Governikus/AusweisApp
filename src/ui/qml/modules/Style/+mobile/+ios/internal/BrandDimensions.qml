/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Window
import Governikus.Global
import Governikus.Type

QtObject {
	readonly property int navigation_bar_bottom_padding: 1
	// If this font size changes, it has to be changed in UiPluginModel, too.
	readonly property real text: scaleText(15) * UiPluginModel.fontScaleFactor
	readonly property real textHeadline: scaleText(26) * UiPluginModel.fontScaleFactor
	readonly property real textSubline: scaleText(18) * UiPluginModel.fontScaleFactor
	readonly property real textTitle: scaleText(30) * UiPluginModel.fontScaleFactor
	readonly property real text_navigation: scaleText(12) * UiPluginModel.fontScaleFactor
	readonly property real text_tile: scaleText(30) * UiPluginModel.fontScaleFactor

	// Scale the text on small devices like the iPhone SE
	function scaleText(value) {
		let w = Screen.width;
		if (w > 415) {
			return value;
		}
		return value * w / 415;
	}
}
