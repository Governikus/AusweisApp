/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Text {
	property TextStyle textStyle: Constants.is_desktop ? Style.text.normal_inverse : Style.text.normal

	Accessible.role: Accessible.StaticText
	Accessible.name: ApplicationModel.stripHtmlTags(text)

	color: textStyle.textColor
	linkColor: textStyle.linkColor
	font.pixelSize: textStyle.textSize

	wrapMode: d.nonMultilineElided ? Text.NoWrap : Text.Wrap

	onLinkActivated: if (link !== '#') Qt.openUrlExternally(link)
	onTextStyleChanged: {
		if (textStyle.textFamily !== "") {
			font.family = textStyle.textFamily
		}
	}
	Keys.onSpacePressed: {
		// Parse the text with a regular expression to see if there's a link somewhere. This has been done by Qt already, but there's no way to access this yet, see https://bugreports.qt.io/browse/QTBUG-79850
		let reg = /href="(.*?)"/g
		let match = reg.exec(text)
		if (!match) { // no link
			return
		}
		let link = match[1]
		if (reg.exec(text)) {
			console.log("More than one link found!")
			return
		}
		linkActivated(link)
	}

	QtObject {
		id: d

		readonly property bool nonMultilineElided: maximumLineCount == 1 && elide !== Text.ElideNone
	}

	MouseArea {
		anchors.fill: parent

		acceptedButtons: Qt.NoButton
		cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : undefined
	}
}
