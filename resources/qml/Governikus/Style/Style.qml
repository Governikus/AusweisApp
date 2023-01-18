/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick 2.15

Item {
	id: style

	readonly property var color: currentTheme.color
	readonly property var currentTheme: plugin.highContrastEnabled ? highContrastTheme : defaultTheme
	readonly property var dimens: currentTheme.dimens
	readonly property var text: currentTheme.text

	QtObject {
		id: highContrastTheme

		readonly property var color: HighContrastColors {
		}
		readonly property var dimens: HighContrastDimensions {
		}
		property bool highContrast: true
		readonly property var text: TextStyles {
		}
	}
	QtObject {
		id: defaultTheme

		readonly property var color: Colors {
		}
		readonly property var dimens: Dimensions {
		}
		readonly property bool highContrast: false
		readonly property var text: TextStyles {
		}
	}
}
