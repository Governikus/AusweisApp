/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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
	readonly property color control_border_hover: "#80cdec"
	readonly property color control_border_pressed: "#0098eb"
	readonly property color control_border_unchecked: "#ffffff"
	readonly property color control_content: "#ffffff"
	readonly property color control_content_disabled: "#798183"
	readonly property color control_content_hover: "#ffffff"
	readonly property color control_content_pressed: "#0098eb"
	readonly property color control_content_unchecked: "#ffffff"
	readonly property color control_disabled: "#303030"
	readonly property color control_hover: "#80cdec"
	readonly property color control_pressed: "#ffffff"
	readonly property color control_unchecked: "#232323"
	readonly property color fail: "#ff9b29"
	readonly property color focus_indicator: "#0098eb"
	readonly property color mainbutton_content_pressed: "#ffffff"
	readonly property color pane: "#303030"
	readonly property color pane_active: "#0098eb"
	readonly property color pane_border: software_renderer ? border : transparent
	readonly property color pane_sublevel: "#576164"
	readonly property color pane_title: "#576164"
	property bool software_renderer: false
	readonly property color success: "#5fcb01"
	readonly property color text: "#ffffff"
	readonly property color text_disabled: "#bcc0c1"
	readonly property color text_headline: "#ffffff"
	readonly property color text_navigation: "#232323"
	readonly property color text_navigation_unchecked: "#80cdec"
	readonly property color text_pressed: "#000000"
	readonly property color text_subline: "#80cdec"
	readonly property color text_subline_disabled: "#798183"
	readonly property color text_subline_pressed: "#000000"
	readonly property color text_title: "#80cdec"
	readonly property color text_warning: "#eb0000"
	readonly property color transparent: "transparent"
}
