/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

Item {
	id: root

	property alias color: delegate.color
	required property string content
	required property int index
	property real maximumContentWidth: Number.POSITIVE_INFINITY
	property alias totalItemCount: delegate.count
	required property int type

	Accessible.ignored: root.content === ""
	Accessible.name: ApplicationModel.stripHtmlTags(root.content)
	Accessible.role: {
		switch (root.type) {
		case FormattedTextModel.LineType.HEADER:
			return Accessible.Heading;
		case FormattedTextModel.LineType.SECTION:
		case FormattedTextModel.LineType.SUBSECTION:
			return Accessible.Section;
		case FormattedTextModel.LineType.LISTITEM:
			return Accessible.ListItem;
		default:
			return Accessible.StaticText;
		}
	}
	implicitHeight: delegate.implicitHeight
	z: 0

	GPaneBackgroundDelegate {
		id: delegate

		anchors.centerIn: parent
		anchors.horizontalCenterOffset: -Style.dimens.pane_padding / 2
		idx: root.index
		implicitHeight: row.implicitHeight
		width: Math.min(root.width - Style.dimens.pane_padding, root.maximumContentWidth)

		RowLayout {
			id: row

			readonly property int horizontalPadding: Style.dimens.pane_padding

			anchors.fill: parent

			GText {
				id: prefix

				Accessible.ignored: true
				Layout.fillHeight: true
				activeFocusOnTab: false
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
				activeFocusOnTab: false
				bottomPadding: delegate.isLast ? Style.dimens.pane_padding : 0
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
				topPadding: delegate.isFirst ? Style.dimens.pane_padding : 0
			}
		}
	}
}
