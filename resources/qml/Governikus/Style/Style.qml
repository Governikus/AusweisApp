/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick 2.12

Item {
	id: style

	QtObject {
		id: highContrastTheme

		readonly property var text: TextStyles {}
		readonly property var color: HighContrastColors {}
		readonly property var dimens: HighContrastDimensions {}

		property bool highContrast: true
	}

	QtObject {
		id: defaultTheme

		readonly property var text: TextStyles {}
		readonly property var color: Colors {}
		readonly property var dimens: Dimensions {}

		readonly property bool highContrast: false
	}

	readonly property var currentTheme: plugin.highContrastEnabled ? highContrastTheme : defaultTheme
	readonly property var text: currentTheme.text
	readonly property var color: currentTheme.color
	readonly property var dimens: currentTheme.dimens
}
