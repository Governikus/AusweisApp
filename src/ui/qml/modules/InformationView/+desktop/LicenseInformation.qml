/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type

GListView {
	id: root

	activeFocusOnTab: true
	anchors.fill: parent
	displayMarginBeginning: Style.dimens.pane_padding
	displayMarginEnd: Style.dimens.pane_padding
	model: ApplicationModel.getLicenseText()

	delegate: ListEntryDelegate {
		z: 0
	}
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: 0
			scope: root
		}
	}

	Keys.onDownPressed: {
		do {
			root.incrementCurrentIndex();
		} while ((currentItem as ListEntryDelegate).text === "")
	}
	Keys.onUpPressed: {
		do {
			root.decrementCurrentIndex();
		} while ((currentItem as ListEntryDelegate).text === "")
	}

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software

		effect: GDropShadow {
		}
	}

	component ListEntryDelegate: RoundedRectangle {
		id: listEntryDelegate

		required property int index
		readonly property bool isFirstItem: index === 0
		readonly property bool isLastItem: index === ListView.view.count - 1
		required property string modelData
		readonly property alias text: delegateText.text

		Accessible.ignored: delegateText.text === ""
		Accessible.name: delegateText.text
		Accessible.role: Accessible.StaticText
		bottomLeftCorner: isLastItem
		bottomRightCorner: isLastItem
		color: Style.color.pane.background.basic
		implicitHeight: Math.ceil(delegateText.implicitHeight) + delegateText.anchors.bottomMargin + delegateText.anchors.topMargin
		topLeftCorner: isFirstItem
		topRightCorner: isFirstItem
		width: root.width - Style.dimens.pane_padding

		GText {
			id: delegateText

			Accessible.ignored: true
			activeFocusOnTab: false
			text: listEntryDelegate.modelData

			anchors {
				bottomMargin: listEntryDelegate.isLastItem ? Style.dimens.pane_padding : 0
				fill: parent
				leftMargin: Style.dimens.pane_padding
				rightMargin: Style.dimens.pane_padding
				topMargin: listEntryDelegate.isFirstItem ? Style.dimens.pane_padding : Style.dimens.text_spacing
			}
		}
	}
}
