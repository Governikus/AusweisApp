/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick

QtObject {
	required property color background
	readonly property color blue: "#164a8c"
	required property color border
	readonly property color card_smart: "#327509"
	required property ControlComponents control
	required property ControlComponents controlCheckbox
	required property ControlComponents controlOptional
	required property ControlComponents controlRadiobutton
	required property ControlComponents controlScrollbar
	required property ControlComponents controlStagedProgressBar
	required property ControlComponents controlSwitch
	required property color error
	required property color focus_indicator
	readonly property color green: "#327509"
	readonly property real highlightDarkerFactor: 1.3
	required property color image
	required property ComponentColors linkBasic
	required property ComponentColors linkNavigation
	required property ComponentColors linkTitle
	required property CoreComponents pane
	required property CoreComponents paneSublevel
	required property color progressbar_text
	required property ComponentColors remoteIndicator
	property bool software_renderer: false
	required property color success
	required property ComponentColors textHeadline
	required property ComponentColors textNormal
	required property ComponentColors textSubline
	required property ComponentColors textTitle
	readonly property color transparent: "transparent"
	required property color warning
	readonly property color white: "#ffffff"

	function darker(pColor, pPercent) {
		return Qt.hsva(pColor.hsvHue, pColor.hsvSaturation, pColor.hsvValue - pPercent, pColor.a);
	}
	function lighter(pColor, pPercent) {
		return Qt.hsva(pColor.hsvHue, pColor.hsvSaturation, pColor.hsvValue + pPercent, pColor.a);
	}
}
