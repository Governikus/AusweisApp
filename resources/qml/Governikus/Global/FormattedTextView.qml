/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.FormattedTextModel

Item {
	id: root

	property alias color: delegate.color
	property alias idx: delegate.idx
	property var lineType
	property real maximumContentWidth: Number.POSITIVE_INFINITY
	property alias text: contentText.text
	property alias totalItemCount: delegate.count

	Accessible.ignored: contentText.text === ""
	Accessible.name: ApplicationModel.stripHtmlTags(contentText.text)
	Accessible.role: {
		switch (root.lineType) {
		case LineType.HEADER:
			return Accessible.Heading;
		case LineType.SECTION:
		case LineType.SUBSECTION:
			return Accessible.Section;
		case LineType.LISTITEM:
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
		anchors.horizontalCenterOffset: -Constants.pane_padding / 2
		implicitHeight: row.implicitHeight
		width: Math.min(root.width - Constants.pane_padding, root.maximumContentWidth)

		RowLayout {
			id: row

			readonly property int horizontalPadding: Constants.pane_padding

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
				visible: root.lineType === LineType.LISTITEM
			}
			GText {
				id: contentText

				Accessible.ignored: true
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				bottomPadding: delegate.isLast ? Constants.pane_padding : 0
				font.underline: lineType === LineType.SECTION
				leftPadding: prefix.visible ? 0 : row.horizontalPadding
				rightPadding: row.horizontalPadding
				textStyle: {
					switch (root.lineType) {
					case LineType.HEADER:
						return Style.text.title;
					case LineType.SECTION:
						return Style.text.headline;
					case LineType.SUBSECTION:
						return Style.text.subline;
					default:
						return Style.text.normal;
					}
				}
				topPadding: delegate.isFirst ? Constants.pane_padding : 0
			}
		}
	}
}
