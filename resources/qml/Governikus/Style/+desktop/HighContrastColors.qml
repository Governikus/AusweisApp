/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

Colors {
	readonly property color background: palette.base
	readonly property color background_disabled: disabledPalette.base
	readonly property color border: control
	readonly property color border_disabled: control_disabled
	readonly property color control: palette.button
	readonly property color control_border: text
	readonly property color control_border_disabled: text_disabled
	readonly property color control_border_hovered: background
	readonly property color control_border_pressed: text_pressed
	readonly property color control_border_unchecked: text
	readonly property color control_content: palette.buttonText
	readonly property color control_content_disabled: text_disabled
	readonly property color control_content_graphics: text
	readonly property color control_content_hovered: background
	readonly property color control_content_pressed: text_pressed
	readonly property color control_content_unchecked: text
	readonly property color control_disabled: disabledPalette.button
	readonly property color control_hovered: text
	readonly property color control_pressed: palette.highlight
	readonly property color control_unchecked: palette.button
	readonly property var disabledPalette: SystemPalette {
		colorGroup: SystemPalette.Disabled
	}
	readonly property color fail: "#ff0000"
	readonly property color focus_indicator: text
	readonly property color image: "#ff0000"
	readonly property var palette: SystemPalette {
		colorGroup: SystemPalette.Active
	}
	readonly property color pane: background
	readonly property color pane_active: pane_pressed
	readonly property color pane_border: text
	readonly property color pane_border_active: pane_border
	readonly property color pane_border_disabled: text_disabled
	readonly property color pane_border_hovered: pane_border
	readonly property color pane_border_pressed: pane_border
	readonly property color pane_border_sublevel: "#576164"
	readonly property color pane_disabled: background_disabled
	readonly property color pane_hovered: text
	readonly property color pane_pressed: palette.highlight
	readonly property color pane_sublevel: background
	readonly property color progressbar_text: text
	readonly property color success: "#00ff00"
	readonly property color text: palette.text
	readonly property color text_active: text_pressed
	readonly property color text_disabled: disabledPalette.text
	readonly property color text_headline: text
	readonly property color text_headline_disabled: text_disabled
	readonly property color text_headline_hovered: background
	readonly property color text_headline_pressed: background
	readonly property color text_hovered: background
	readonly property color text_pressed: palette.highlightedText
	readonly property color text_subline: text
	readonly property color text_subline_disabled: text_disabled
	readonly property color text_subline_hovered: background
	readonly property color text_subline_pressed: text_pressed
	readonly property color text_title: text
	readonly property color text_title_disabled: text_disabled
	readonly property color text_title_focus: text
	readonly property color text_title_hovered: background
	readonly property color text_title_pressed: text_pressed
	readonly property color warning: "#ff0000"
}
