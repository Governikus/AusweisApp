/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

GPaneBackgroundDelegate {
	id: root

	required property string content
	required property int index
	required property int type

	signal scrollDownAction
	signal scrollUpAction

	Accessible.focusable: true
	Accessible.ignored: root.content === ""
	Accessible.name: ApplicationModel.stripHtmlTags(root.content)
	Accessible.role: {
		switch (root.type) {
		case FormattedTextModel.LineType.HEADER:
		case FormattedTextModel.LineType.SECTION:
		case FormattedTextModel.LineType.SUBSECTION:
			return Utils.useSpecialAppleTabRole(Accessible.Heading);
		default:
			return Qt.platform.os === "windows" ? Accessible.Paragraph : Utils.useSpecialAppleTabRole(Accessible.StaticText);
		}
	}
	idx: index
	implicitHeight: row.implicitHeight

	Accessible.onScrollDownAction: root.scrollDownAction()
	Accessible.onScrollUpAction: root.scrollUpAction()

	RowLayout {
		id: row

		readonly property int horizontalPadding: Style.dimens.pane_padding

		anchors.fill: parent

		GText {
			id: prefix

			Accessible.ignored: true
			Layout.fillHeight: true
			fontSizeMode: Text.Fit
			leftPadding: row.horizontalPadding
			text: "•"
			textStyle: contentText.textStyle
			verticalAlignment: Text.AlignTop
			visible: root.type === FormattedTextModel.LineType.LISTITEM
		}
		GText {
			id: contentText

			Accessible.ignored: true
			Layout.maximumWidth: Number.POSITIVE_INFINITY
			bottomPadding: root.isLast ? Style.dimens.pane_padding : 0
			font.underline: root.type === FormattedTextModel.LineType.SECTION
			leftPadding: prefix.visible ? 0 : row.horizontalPadding
			rightPadding: row.horizontalPadding
			text: root.content
			textStyle: {
				switch (root.type) {
				case FormattedTextModel.LineType.HEADER:
					return Style.text.title;
				case FormattedTextModel.LineType.SECTION:
					return Style.text.headline;
				case FormattedTextModel.LineType.SUBSECTION:
					return Style.text.subline;
				default:
					return Style.text.normal;
				}
			}
			topPadding: root.isFirst ? Style.dimens.pane_padding : 0
		}
	}
}
