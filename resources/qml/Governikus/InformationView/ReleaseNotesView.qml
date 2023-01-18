/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
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

	delegate: RowLayout {
		id: row

		readonly property alias text: contentText.text

		Accessible.ignored: contentText.text === ""
		Accessible.name: ApplicationModel.stripHtmlTags(contentText.text)
		Accessible.role: Accessible.StaticText
		width: root.width - Constants.pane_padding

		GText {
			Accessible.ignored: true
			Layout.fillHeight: true
			fontSizeMode: Text.Fit
			text: "•"
			textStyle: contentText.textStyle
			verticalAlignment: Text.AlignTop
			visible: lineType === LineType.LISTITEM
		}
		GText {
			id: contentText
			Accessible.ignored: true
			Layout.fillWidth: true
			font.underline: lineType === LineType.SECTION
			text: content
			textStyle: {
				switch (lineType) {
				case LineType.HEADER:
					return Style.text.title;
				case LineType.SECTION:
					return Style.text.header;
				case LineType.SUBSECTION:
					return Style.text.header_accent;
				default:
					return Style.text.normal;
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
}
