/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.View
import Governikus.Style

GListView {
	id: root

	property real maximumContentWidth: Number.POSITIVE_INFINITY

	Accessible.ignored: false
	activeFocusOnTab: true
	displayMarginBeginning: Style.dimens.pane_padding
	displayMarginEnd: Style.dimens.pane_padding

	delegate: FormattedTextView {
		maximumContentWidth: root.maximumContentWidth
		totalItemCount: root.count
		width: root.width
	}
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: Style.dimens.pane_padding
			scope: root
		}
	}

	Keys.onDownPressed: {
		do {
			root.incrementCurrentIndex();
		} while ((currentItem as FormattedTextView).content === "")
	}
	Keys.onUpPressed: {
		do {
			root.decrementCurrentIndex();
		} while ((currentItem as FormattedTextView).content === "")
	}

	layer {
		enabled: true

		effect: GDropShadow {
		}
	}
}
