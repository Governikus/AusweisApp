/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


GButton {
	readonly property color pressColor: Qt.darker(textStyle.textColor, Constants.highlightDarkerFactor)

	visible: ApplicationModel.currentWorkflow !== ""

	text: qsTr("Cancel") + SettingsModel.translationTrigger
	textStyle: Style.text.header_inverse
	buttonColor: Style.color.transparent
	icon.source: "qrc:///images/cancel.svg"
	font.bold: true
	tintIcon: true
	padding: 0
	leftPadding: Constants.component_spacing
	rightPadding: Constants.component_spacing
	textHighlightColor: pressed ? pressColor : textStyle.textColor

	GSeparator {
		visible: !Style.currentTheme.highContrast

		height: parent.height

		orientation: Qt.Vertical
		color: Style.text.header_inverse.textColor
	}
}
