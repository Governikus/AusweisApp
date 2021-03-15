/*
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.FormattedTextModel 1.0


GListView {
	id: root

	activeFocusOnTab: true
	displayMarginBeginning: Constants.pane_padding
	displayMarginEnd: Constants.pane_padding
	delegate: RowLayout {
		width: root.width - Constants.pane_padding

		GText {
			visible: lineType === FormattedTextModel.LISTITEM
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
			text: content
			textStyle: {
				switch (lineType) {
				case FormattedTextModel.HEADER:
					return Style.text.title
				case FormattedTextModel.SECTION:
					return Style.text.header
				case FormattedTextModel.SUBSECTION:
					return Style.text.header_accent
				default:
					return Style.text.normal
				}
			}
			font.underline: lineType === FormattedTextModel.SECTION
		}
	}

	FocusFrame {
		borderColor: Style.color.focus_indicator
	}
}
