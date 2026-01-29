/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style

GPane {
	id: root

	property alias buttonIconSource: hintButton.icon.source
	property alias buttonText: hintButton.text
	property alias buttonTooltip: hintButton.enabledTooltipText
	property string linkToOpen
	property alias text: hintText.text

	signal clicked
	signal linkAboutToOpen

	Accessible.ignored: true
	color: Style.color.paneSublevel.background.basic_unchecked
	contentSpacing: 0
	drawShadow: false

	GText {
		id: hintText

		Layout.topMargin: Style.dimens.text_spacing
		visible: text !== ""
	}
	GButton {
		id: hintButton

		readonly property bool hasLink: root.linkToOpen !== ""

		Accessible.description: hasLink ? Utils.platformAgnosticLinkOpenText(root.linkToOpen, Accessible.name) : ""
		Accessible.role: hasLink ? Accessible.Link : Accessible.Button
		Layout.alignment: Qt.AlignHCenter
		Layout.topMargin: Style.dimens.pane_spacing
		icon.source: "qrc:///images/open_website.svg"
		tintIcon: true
		visible: text !== ""

		onClicked: {
			if (hasLink) {
				root.linkAboutToOpen();
				Qt.openUrlExternally(root.linkToOpen);
			} else {
				root.clicked();
			}
		}
	}
}
