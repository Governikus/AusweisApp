/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

GButton {
	readonly property color pressColor: Qt.darker(textStyle.textColor, Constants.highlightDarkerFactor)

	buttonColor: Style.color.transparent
	font.bold: true
	height: if (parent)
		parent.height
	icon.source: "qrc:///images/material_close.svg"
	text: qsTr("Cancel")
	textHighlightColor: pressed ? pressColor : textStyle.textColor
	textStyle: Style.text.header_inverse
	tintIcon: true
	verticalPadding: 0
	visible: ApplicationModel.currentWorkflow !== ApplicationModel.WORKFLOW_NONE

	GSeparator {
		color: Style.text.header_inverse.textColor
		height: parent.height
		orientation: Qt.Vertical
		visible: !Style.currentTheme.highContrast
	}
}
