/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

Colors {
	readonly property var palette: SystemPalette { colorGroup: SystemPalette.Active }
	readonly property var disabledPalette: SystemPalette { colorGroup: SystemPalette.Disabled }

	readonly property color shadow: transparent
	readonly property color high_contrast_item_border: palette.text
	readonly property color border: palette.text

	readonly property color primary_text: palette.text
	readonly property color primary_text_inverse: palette.text
	readonly property color secondary_text: palette.text
	readonly property color secondary_text_inverse: palette.text
	readonly property color accent_text: palette.text
	readonly property color accent_text_inverse: palette.text
	readonly property color warning_text: "#ff0000"

	readonly property color background: palette.base
	readonly property color background_popup: background
	readonly property color background_pane: background
	readonly property color background_pane_active: background
	readonly property color background_pane_inactive: background

	readonly property color accent: palette.highlight

	readonly property color button: palette.button
	readonly property color button_disabled: disabledPalette.button
	readonly property color button_text: palette.text
	readonly property color button_text_disabled: disabledPalette.text

	readonly property color navigation: background_pane

	readonly property color focus_indicator: primary_text
	readonly property color focus_indicator_inverse: primary_text_inverse
}
