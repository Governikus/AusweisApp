/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type
import Governikus.View

Text {
	id: root

	readonly property string a11yName: ApplicationModel.stripHtmlTags(text)
	readonly property real effectiveFirstLineHeight: topPadding + Math.ceil(lineHeight) + bottomPadding
	readonly property real effectiveMaxLinesHeight: topPadding + maximumLineCount * Math.ceil(lineHeight) + bottomPadding
	property alias focusFrameVisible: focusFrame.visible
	readonly property bool hasLink: link !== ""
	readonly property alias link: d.link
	property TextStyle textStyle: Style.text.normal

	function tryActivateLink() {
		if (hasLink) {
			linkActivated(d.link);
		}
	}

	Accessible.focusable: true
	Accessible.ignored: text === ""
	Accessible.name: hasLink ? Utils.platformAgnosticLinkOpenText(link, a11yName) : a11yName
	Accessible.role: hasLink ? Accessible.Link : Accessible.StaticText
	Layout.fillWidth: true
	Layout.maximumWidth: Math.ceil(implicitWidth)
	activeFocusOnTab: hasLink
	color: textStyle.textColor
	font.family: UiPluginModel.fontFamily
	font.pixelSize: textStyle.textSize
	font.weight: textStyle.fontWeight
	lineHeight: textStyle.lineHeight
	lineHeightMode: Text.FixedHeight
	linkColor: color
	verticalAlignment: Text.AlignVCenter
	wrapMode: d.nonMultilineElided ? Text.NoWrap : Text.Wrap

	Accessible.onPressAction: tryActivateLink()
	Accessible.onScrollDownAction: Utils.scrollPageDownOnGFlickable(this)
	Accessible.onScrollUpAction: Utils.scrollPageUpOnGFlickable(this)
	Accessible.onShowOnScreenAction: Utils.positionViewAtItem(this)
	Component.onCompleted: d.checkForLinks()
	Keys.onEnterPressed: tryActivateLink()
	Keys.onReturnPressed: tryActivateLink()
	Keys.onSpacePressed: tryActivateLink()
	onActiveFocusChanged: if (activeFocus)
		Utils.positionViewAtItem(this)
	onLinkActivated: pLink => {
		Qt.openUrlExternally(pLink);
	}
	onTextChanged: d.checkForLinks()

	FocusFrame {
		id: focusFrame
	}
	QtObject {
		id: d

		readonly property bool isHeadline: [Style.text.headline, Style.text.subline].includes(root.textStyle)
		property string link: ""
		readonly property bool nonMultilineElided: root.maximumLineCount === 1 && root.elide !== Text.ElideNone

		function checkForLinks() {
			// Parse the text with a regular expression to see if there's a link somewhere. This has been done by Qt already, but there's no way to access this yet, see https://bugreports.qt.io/browse/QTBUG-79850
			let match = root.text.match(/href=".*?"/g);
			if (match) {
				// 6 = href="
				// -1 = "
				d.link = match[0].substring(6, match[0].length - 1);
				if (match.length > 1) {
					console.warn("More than one link found:", match);
				}
			} else {
				d.link = "";
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
		GToolTip {
			text: root.hoveredLink
			visible: Style.is_layout_desktop && root.hoveredLink !== ""

			onAboutToShow: {
				parent.x = mouseArea.mouseX;
				parent.y = mouseArea.mouseY;
			}
		}
	}
}
