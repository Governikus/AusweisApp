/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.View
import Governikus.Style

GListView {
	id: root

	property real maximumContentWidth: Number.POSITIVE_INFINITY

	function isListElementEmptyFunc(pItem) {
		let delegate = pItem as FormattedTextView;
		if (delegate)
			return delegate.content === "";
		return true;
	}

	displayMarginBeginning: Style.dimens.pane_padding
	displayMarginEnd: Style.dimens.pane_padding

	delegate: FormattedTextView {
		anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined
		anchors.horizontalCenterOffset: -Style.dimens.pane_padding / 2
		count: root.count
		width: Math.min(root.width - Style.dimens.pane_padding, root.maximumContentWidth)

		onActiveFocusChanged: if (activeFocus) {
			root.handleItemFocused(index);
		}
		onScrollDownAction: root.scrollPageDown()
		onScrollUpAction: root.scrollPageUp()
	}
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: 0
			scope: root
			visible: Style.is_layout_desktop
		}
	}

	layer {
		enabled: true

		effect: GDropShadow {
		}
	}
}
