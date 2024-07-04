/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick
import Governikus.Type

Item {
	id: style

	readonly property var color: currentTheme.color
	readonly property var currentTheme: UiPluginModel.highContrastEnabled ? highContrastTheme : useDarkMode ? darkTheme : defaultTheme
	readonly property var dimens: currentTheme.dimens
	property bool software_renderer: false
	readonly property var text: currentTheme.text
	readonly property bool useDarkMode: (UiPluginModel.osDarkModeEnabled && SettingsModel.userDarkMode === SettingsModel.ModeOption.AUTO) || SettingsModel.userDarkMode === SettingsModel.ModeOption.ON

	QtObject {
		id: darkTheme

		readonly property var color: PlatformColorsDark {
			software_renderer: style.software_renderer
		}
		readonly property alias dimens: dimensions
		readonly property bool highContrast: false
		readonly property string name: "darkmode"
		readonly property alias text: textStyles
	}
	QtObject {
		id: highContrastTheme

		readonly property var color: PlatformColorsContrast {
			software_renderer: style.software_renderer
		}
		readonly property alias dimens: dimensions
		property bool highContrast: true
		readonly property string name: "highcontrast"
		readonly property alias text: textStyles
	}
	QtObject {
		id: defaultTheme

		readonly property var color: PlatformColorsLight {
			software_renderer: style.software_renderer
		}
		readonly property alias dimens: dimensions
		readonly property bool highContrast: false
		readonly property string name: "lightmode"
		readonly property alias text: textStyles
	}
	Dimensions {
		id: dimensions

	}
	TextStyles {
		id: textStyles

	}
}
