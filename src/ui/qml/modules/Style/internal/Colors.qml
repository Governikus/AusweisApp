/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	required property color background
	required property color border
	readonly property color card_eid: "#0077b6"
	readonly property color card_smart: "#327509"
	required property ControlComponents control
	required property ControlComponents controlCheckbox
	required property ControlComponents controlOptional
	required property ControlComponents controlRadiobutton
	required property ControlComponents controlScrollbar
	required property ControlComponents controlSwitch
	readonly property color error: "#eb0000"
	required property color fail
	required property color focus_indicator
	required property color image
	required property ComponentColors linkBasic
	required property ComponentColors linkNavigation
	required property ComponentColors linkTitle
	required property CoreComponents pane
	required property CoreComponents paneSublevel
	required property color progressbar_text
	property bool software_renderer: false
	required property color success
	required property ComponentColors textHeadline
	required property ComponentColors textNormal
	required property ComponentColors textSubline
	required property ComponentColors textTitle
	readonly property color transparent: "transparent"
	required property color warning

	function darker(pColor, pPercent) {
		return Qt.hsva(pColor.hsvHue, pColor.hsvSaturation, pColor.hsvValue - pPercent, pColor.a);
	}
	function lighter(pColor, pPercent) {
		return Qt.hsva(pColor.hsvHue, pColor.hsvSaturation, pColor.hsvValue + pPercent, pColor.a);
	}
}
