/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import Governikus.Style 1.0

Text {
	property TextStyle textStyle: Style.text.normal

	Accessible.role: Accessible.StaticText
	Accessible.name: text

	color: textStyle.textColor
	font.pixelSize: textStyle.textSize

	wrapMode: Text.Wrap

	onTextStyleChanged: {
		if (textStyle.textFamily !== "") {
			font.family = textStyle.textFamily
		}
	}

	MouseArea {
		anchors.fill: parent

		acceptedButtons: Qt.NoButton
		cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
	}
}
