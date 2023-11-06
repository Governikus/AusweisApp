/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Colors {
	readonly property color background: palette.base
	readonly property color border: palette.text
	readonly property color control: palette.button
	readonly property color control_border: palette.text
	readonly property color control_border_disabled: disabledPalette.text
	readonly property color control_border_hover: palette.text
	readonly property color control_border_pressed: palette.text
	readonly property color control_border_unchecked: palette.text
	readonly property color control_content: palette.text
	readonly property color control_content_disabled: disabledPalette.text
	readonly property color control_content_hover: palette.text
	readonly property color control_content_pressed: palette.text
	readonly property color control_content_unchecked: palette.text
	readonly property color control_disabled: disabledPalette.button
	readonly property color control_hover: palette.button
	readonly property color control_pressed: palette.button
	readonly property color control_unchecked: palette.button
	readonly property var disabledPalette: SystemPalette {
		colorGroup: SystemPalette.Disabled
	}
	readonly property color fail: "#ff0000"
	readonly property color focus_indicator: text
	readonly property color mainbutton_content_pressed: palette.text
	readonly property var palette: SystemPalette {
		colorGroup: SystemPalette.Active
	}
	readonly property color pane: background
	readonly property color pane_active: background
	readonly property color pane_border: palette.text
	readonly property color pane_sublevel: background
	readonly property color success: "#00ff00"
	readonly property color text: palette.text
	readonly property color text_disabled: disabledPalette.text
	readonly property color text_headline: palette.text
	readonly property color text_pressed: palette.text
	readonly property color text_subline: palette.text
	readonly property color text_subline_disabled: disabledPalette.text
	readonly property color text_subline_pressed: palette.text
	readonly property color text_title: palette.text
	readonly property color text_warning: "#ff0000"
}
