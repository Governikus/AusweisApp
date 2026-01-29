/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

GListView {
	id: root

	function isListElementEmptyFunc(pItem) {
		let delegate = pItem as ListEntryDelegate;
		if (delegate)
			return delegate.text === "";
		return true;
	}

	anchors.fill: parent
	displayMarginBeginning: Style.dimens.pane_padding
	displayMarginEnd: Style.dimens.pane_padding
	model: ApplicationModel.getLicenseText()

	delegate: ListEntryDelegate {
		z: 0

		onActiveFocusChanged: if (activeFocus) {
			root.handleItemFocused(index);
		}
	}
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: 0
			scope: root
		}
	}

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software

		effect: GDropShadow {
		}
	}

	component ListEntryDelegate: GPaneBackgroundDelegate {
		id: listEntryDelegate

		required property int index
		required property string modelData
		readonly property alias text: delegateText.text

		Accessible.focusable: true
		Accessible.ignored: delegateText.text === ""
		Accessible.name: delegateText.text
		Accessible.role: Utils.useSpecialAppleTabRole(Accessible.StaticText)
		count: root.count
		idx: index
		implicitHeight: Math.ceil(delegateText.implicitHeight) + delegateText.anchors.bottomMargin + delegateText.anchors.topMargin
		width: root.width - Style.dimens.pane_padding

		GText {
			id: delegateText

			Accessible.ignored: true
			text: listEntryDelegate.modelData

			anchors {
				bottomMargin: listEntryDelegate.isLast ? Style.dimens.pane_padding : 0
				fill: parent
				leftMargin: Style.dimens.pane_padding
				rightMargin: Style.dimens.pane_padding
				topMargin: listEntryDelegate.isFirst ? Style.dimens.pane_padding : Style.dimens.text_spacing
			}
		}
	}
}
