/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel

Text {
	id: root

	readonly property real effectiveFirstLineHeight: topPadding + Math.ceil(lineHeight) + bottomPadding
	readonly property real effectiveMaxLinesHeight: topPadding + maximumLineCount * Math.ceil(lineHeight) + bottomPadding
	property TextStyle textStyle: Style.text.normal

	Accessible.ignored: text === ""
	//: INFO ALL_PLATFORMS Text read by screen reader if the text contains a weblink which may be opened.
	Accessible.name: ApplicationModel.stripHtmlTags(text) + (Constants.is_desktop && d.hasLink ? " " + qsTr("Press space to open link: %1").arg(d.link) : "")
	Accessible.role: Accessible.StaticText
	Layout.fillWidth: true
	Layout.maximumWidth: Math.ceil(implicitWidth)
	color: textStyle.textColor
	font.pixelSize: textStyle.textSize
	lineHeight: textStyle.lineHeight
	lineHeightMode: Text.FixedHeight
	linkColor: color
	verticalAlignment: Text.AlignVCenter
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
			text: root.hoveredLink
			visible: Constants.is_desktop && root.hoveredLink !== ""

			onAboutToShow: {
				parent.x = mouseArea.mouseX;
				parent.y = mouseArea.mouseY;
			}
		}
	}
}
