/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	readonly property color background: "#232323"
	readonly property color border: "#ffffff"
	readonly property color card_eid: "#0077b6"
	readonly property color card_reader: "#444445"
	readonly property color card_smart: "#327509"
	readonly property color control: "#0098eb"
	readonly property color control_border: "#0098eb"
	readonly property color control_border_disabled: "#798183"
	readonly property color control_border_hovered: "#80cdec"
	readonly property color control_border_pressed: "#0098eb"
	readonly property color control_border_unchecked: "#ffffff"
	readonly property color control_content: "#232323"
	readonly property color control_content_disabled: "#798183"
	readonly property color control_content_graphics: "#ffffff"
	readonly property color control_content_hovered: "#ffffff"
	readonly property color control_content_pressed: "#0098eb"
	readonly property color control_content_unchecked: "#ffffff"
	readonly property color control_disabled: "#303030"
	readonly property color control_hovered: "#80cdec"
	readonly property color control_pressed: transparent
	readonly property color control_unchecked: "#232323"
	readonly property color fail: "#ff9b29"
	readonly property color focus_indicator: "#0098eb"
	readonly property color image: "#0098eb"
	readonly property color pane: "#303030"
	readonly property color pane_active: "#0098eb"
	readonly property color pane_border: software_renderer ? border : transparent
	readonly property color pane_border_active: pane_border
	readonly property color pane_border_disabled: pane_border
	readonly property color pane_border_hovered: pane_border
	readonly property color pane_border_pressed: pane_border
	readonly property color pane_border_sublevel: "#576164"
	readonly property color pane_disabled: "#303030"
	readonly property color pane_hovered: "#0098eb"
	readonly property color pane_pressed: "#80cdec"
	readonly property color pane_sublevel: "#576164"
	readonly property color progressbar_text: text
	property bool software_renderer: false
	readonly property color success: "#5fcb01"
	readonly property color text: "#ffffff"
	readonly property color text_active: "#000000"
	readonly property color text_disabled: "#bcc0c1"
	readonly property color text_headline: "#ffffff"
	readonly property color text_headline_disabled: "#bcc0c1"
	readonly property color text_headline_hovered: "#000000"
	readonly property color text_headline_pressed: "#000000"
	readonly property color text_hovered: "#000000"
	readonly property color text_pressed: "#000000"
	readonly property color text_subline: "#80cdec"
	readonly property color text_subline_disabled: "#798183"
	readonly property color text_subline_hovered: "#000000"
	readonly property color text_subline_pressed: "#000000"
	readonly property color text_title: "#80cdec"
	readonly property color text_title_disabled: "#798183"
	readonly property color text_title_focus: "#ffffff"
	readonly property color text_title_hovered: "#ffffff"
	readonly property color text_title_pressed: "#ffffff"
	readonly property color transparent: "transparent"
	readonly property color warning: "#eb0000"
}
