/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0

Text {
	id: baseItem

	property TextStyle textStyle: Style.text.normal

	Accessible.ignored: text === ""
	//: INFO ALL_PLATFORMS Text read by screen reader if the text contains a weblink which may be opened.
	Accessible.name: ApplicationModel.stripHtmlTags(text) + (Constants.is_desktop && d.hasLink ? " " + qsTr("Press space to open link: %1").arg(d.link) : "")
	Accessible.role: Accessible.StaticText
	color: textStyle.textColor
	font.bold: textStyle.bold
	font.italic: textStyle.italic
	font.pixelSize: textStyle.textSize
	font.underline: textStyle.underline
	linkColor: textStyle.linkColor
	wrapMode: d.nonMultilineElided ? Text.NoWrap : Text.Wrap

	Component.onCompleted: d.checkForLinks()
	Keys.onSpacePressed: {
		if (d.hasLink) {
			linkActivated(d.link);
		}
	}
	onLinkActivated: pLink => {
		Qt.openUrlExternally(pLink);
	}
	onTextChanged: d.checkForLinks()
	onTextStyleChanged: {
		if (textStyle.textFamily !== "") {
			font.family = textStyle.textFamily;
		}
	}

	QtObject {
		id: d

		readonly property bool hasLink: link !== ""
		property string link: ""
		readonly property bool nonMultilineElided: maximumLineCount === 1 && elide !== Text.ElideNone

		function checkForLinks() {
			// Parse the text with a regular expression to see if there's a link somewhere. This has been done by Qt already, but there's no way to access this yet, see https://bugreports.qt.io/browse/QTBUG-79850
			let reg = /href="(.*?)"/g;
			let match = reg.exec(text);
			if (!match) {
				// no link
				d.link = "";
				return;
			}
			d.link = match[1];
			if (reg.exec(text)) {
				console.warn("More than one link found!");
			}
		}
	}
	MouseArea {
		id: mouseArea
		acceptedButtons: Qt.NoButton
		anchors.fill: parent
		cursorShape: parent.hoveredLink !== "" ? Qt.PointingHandCursor : undefined
		hoverEnabled: true
	}
	Item {
		ToolTip {
			delay: Constants.toolTipDelay
			text: baseItem.hoveredLink
			visible: Constants.is_desktop && baseItem.hoveredLink !== ""

			onAboutToShow: {
				parent.x = mouseArea.mouseX;
				parent.y = mouseArea.mouseY;
			}
		}
	}
}
