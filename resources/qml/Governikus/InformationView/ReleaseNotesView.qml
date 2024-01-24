/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.FormattedTextModel

GListView {
	id: root

	property real maximumContentWidth: Number.POSITIVE_INFINITY

	activeFocusOnTab: true
	displayMarginBeginning: Constants.pane_padding
	displayMarginEnd: Constants.pane_padding

	delegate: Item {
		height: row.implicitHeight
		width: root.width

		GPaneBackgroundDelegate {
			id: delegate

			anchors.centerIn: parent
			anchors.horizontalCenterOffset: -Constants.pane_padding / 2
			count: root.count
			idx: index
			implicitHeight: row.implicitHeight
			implicitWidth: Math.min(root.width - Constants.pane_padding, root.maximumContentWidth)

			RowLayout {
				id: row

				readonly property int horizontalPadding: Constants.pane_padding
				readonly property alias text: contentText.text

				Accessible.ignored: contentText.text === ""
				Accessible.name: ApplicationModel.stripHtmlTags(contentText.text)
				Accessible.role: Accessible.StaticText
				anchors.fill: parent
				implicitHeight: Math.max(prefix.implicitHeight, contentText.implicitHeight)

				GText {
					id: prefix

					Accessible.ignored: true
					Layout.fillHeight: true
					fontSizeMode: Text.Fit
					leftPadding: row.horizontalPadding
					text: "•"
					textStyle: contentText.textStyle
					verticalAlignment: Text.AlignTop
					visible: lineType === LineType.LISTITEM
				}
				GText {
					id: contentText

					Accessible.ignored: true
					Layout.maximumWidth: Number.POSITIVE_INFINITY
					bottomPadding: delegate.isLast ? Constants.pane_padding : 0
					font.underline: lineType === LineType.SECTION
					leftPadding: prefix.visible ? 0 : row.horizontalPadding
					rightPadding: row.horizontalPadding
					text: content
					textStyle: {
						switch (lineType) {
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
	highlight: Item {
		FocusFrame {
			scope: root
		}
	}

	Keys.onDownPressed: {
		do {
			root.incrementCurrentIndex();
		} while (currentItem.text === "")
	}
	Keys.onUpPressed: {
		do {
			root.decrementCurrentIndex();
		} while (currentItem.text === "")
	}

	layer {
		enabled: true

		effect: GDropShadow {
		}
	}
}
