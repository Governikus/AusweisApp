/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

GListView {
	id: root

	property real maximumContentWidth: Number.POSITIVE_INFINITY

	Accessible.ignored: false
	activeFocusOnTab: true
	displayMarginBeginning: Constants.pane_padding
	displayMarginEnd: Constants.pane_padding

	delegate: FormattedTextView {
		idx: index
		lineType: model.lineType
		maximumContentWidth: root.maximumContentWidth
		text: model.content
		totalItemCount: count
		width: root.width
	}
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: Constants.pane_padding
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
