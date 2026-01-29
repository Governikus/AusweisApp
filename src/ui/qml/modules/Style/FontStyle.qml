/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

import Governikus.Type

QtObject {
	id: root

	readonly property int bold: root.adjustFontWeight(Font.Bold)
	readonly property int medium: root.adjustFontWeight(Font.Medium)
	readonly property int normal: root.adjustFontWeight(Font.Normal)

	function adjustFontWeight(fontWeight) {
		fontWeight += UiPluginModel.fontWeightAdjustment;
		fontWeight = Math.max(1, fontWeight);
		fontWeight = Math.min(fontWeight, 1000);
		return fontWeight;
	}
}
