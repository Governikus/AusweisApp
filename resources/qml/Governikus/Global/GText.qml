/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Text {
	property TextStyle textStyle: Style.text.normal

	Accessible.role: Accessible.StaticText
	//: INFO ALL_PLATFORMS Text read by screen reader if the text contains a weblink which may be opened.
	Accessible.name: ApplicationModel.stripHtmlTags(text) +
		(Constants.is_desktop && d.hasLink ? " " + qsTr("Press space to open link: %1").arg(d.link) : "")
	Accessible.ignored: text === ""

	color: textStyle.textColor
	linkColor: textStyle.linkColor
	font.pixelSize: textStyle.textSize

	wrapMode: d.nonMultilineElided ? Text.NoWrap : Text.Wrap

	onLinkActivated: pLink => { Qt.openUrlExternally(pLink) }
	onTextStyleChanged: {
		if (textStyle.textFamily !== "") {
			font.family = textStyle.textFamily
		}
	}
	onTextChanged: d.checkForLinks()
	Component.onCompleted: d.checkForLinks()
	Keys.onSpacePressed: {
		if (d.hasLink) {
			linkActivated(d.link)
		}
	}

	QtObject {
		id: d

		readonly property bool nonMultilineElided: maximumLineCount === 1 && elide !== Text.ElideNone

		property string link: ""
		readonly property bool hasLink: link !== ""

		function checkForLinks() {
			// Parse the text with a regular expression to see if there's a link somewhere. This has been done by Qt already, but there's no way to access this yet, see https://bugreports.qt.io/browse/QTBUG-79850
			let reg = /href="(.*?)"/g
			let match = reg.exec(text)
			if (!match) { // no link
				d.link = ""
				return
			}
			d.link = match[1]
			if (reg.exec(text)) {
				console.warn("More than one link found!")
			}
		}
	}

	MouseArea {
		anchors.fill: parent

		acceptedButtons: Qt.NoButton
		cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : undefined
	}
}
