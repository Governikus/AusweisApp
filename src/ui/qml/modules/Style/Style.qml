/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
pragma Singleton
import QtQuick
import Governikus.Type

PlatformStyle {
	id: root

	readonly property int animation_duration: 250
	readonly property var color: currentTheme.color
	readonly property var currentTheme: UiPluginModel.highContrastEnabled ? highContrastTheme : UiPluginModel.darkModeEnabled ? darkTheme : defaultTheme
	readonly property var dimens: currentTheme.dimens
	readonly property var flickDeceleration: Style.is_layout_desktop ? 7500.0 : 1500.0
	readonly property double scrolling_speed: 7500.0
	property bool software_renderer: false
	readonly property var text: currentTheme.text
	readonly property int toolTipDelay: 500

	QtObject {
		id: darkTheme

		readonly property var color: PlatformColorsDark {
			software_renderer: root.software_renderer
		}
		readonly property alias dimens: dimensions
		readonly property bool highContrast: false
		readonly property alias text: textStyles
	}
	QtObject {
		id: highContrastTheme

		readonly property var color: PlatformColorsContrast {
			software_renderer: root.software_renderer
		}
		readonly property alias dimens: dimensions
		property bool highContrast: true
		readonly property alias text: textStyles
	}
	QtObject {
		id: defaultTheme

		readonly property var color: PlatformColorsLight {
			software_renderer: root.software_renderer
		}
		readonly property alias dimens: dimensions
		readonly property bool highContrast: false
		readonly property alias text: textStyles
	}
	Dimensions {
		id: dimensions

	}
	TextStyles {
		id: textStyles

	}
}
