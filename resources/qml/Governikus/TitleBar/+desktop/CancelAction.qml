/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

GButton {
	readonly property color pressColor: Qt.darker(textStyle.textColor, Constants.highlightDarkerFactor)

	visible: ApplicationModel.currentWorkflow !== ""
	height: if (parent) parent.height

	text: qsTr("Cancel")
	textStyle: Style.text.header_inverse
	buttonColor: Style.color.transparent
	icon.source: "qrc:///images/material_close.svg"
	font.bold: true
	tintIcon: true
	verticalPadding: 0
	textHighlightColor: pressed ? pressColor : textStyle.textColor

	GSeparator {
		visible: !Style.currentTheme.highContrast

		height: parent.height

		orientation: Qt.Vertical
		color: Style.text.header_inverse.textColor
	}
}
