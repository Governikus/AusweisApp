/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.FormattedTextModel 1.0


GListView {
	id: root

	activeFocusOnTab: true

	displayMarginBeginning: Constants.pane_padding
	displayMarginEnd: Constants.pane_padding

	Keys.onDownPressed: {
		do {
			root.incrementCurrentIndex()
		} while (currentItem.text === "")
	}
	Keys.onUpPressed: {
		do {
			root.decrementCurrentIndex()
		} while (currentItem.text === "")
	}

	highlight: Item {
		FocusFrame {
			scope: root
		}
	}

	delegate: RowLayout {
		id: row

		readonly property alias text: contentText.text

		width: root.width - Constants.pane_padding

		Accessible.role: Accessible.StaticText
		Accessible.name: ApplicationModel.stripHtmlTags(contentText.text)
		Accessible.ignored: contentText.text === ""

		GText {
			visible: lineType === LineType.LISTITEM
			Layout.fillHeight: true

			Accessible.ignored: true

			verticalAlignment: Text.AlignTop
			fontSizeMode: Text.Fit
			text: "•"
			textStyle: contentText.textStyle
		}

		GText {
			id: contentText

			Layout.fillWidth: true

			Accessible.ignored: true

			text: content
			textStyle: {
				switch (lineType) {
				case LineType.HEADER:
					return Style.text.title
				case LineType.SECTION:
					return Style.text.header
				case LineType.SUBSECTION:
					return Style.text.header_accent
				default:
					return Style.text.normal
				}
			}
			font.underline: lineType === LineType.SECTION
		}
	}
}
