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
	id: listView

	activeFocusOnTab: true
	anchors.fill: parent
	displayMarginBeginning: Constants.pane_padding
	displayMarginEnd: Constants.pane_padding
	model: ApplicationModel.getLicenseText()

	delegate: RoundedRectangle {
		readonly property bool isFirstItem: index === 0
		readonly property bool isLastItem: index === ListView.view.count - 1
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
		width: listView.width - Constants.pane_padding
		z: 0

		GText {
			id: delegateText

			Accessible.ignored: true
			text: model.modelData

			anchors {
				bottomMargin: isLastItem ? Constants.pane_padding : 0
				fill: parent
				leftMargin: Constants.pane_padding
				rightMargin: Constants.pane_padding
				topMargin: isFirstItem ? Constants.pane_padding : Constants.text_spacing
			}
		}
	}
	highlight: Item {
		z: 2

		FocusFrame {
			anchors.leftMargin: 0
			anchors.rightMargin: 0
			scope: listView
		}
	}

	Keys.onDownPressed: {
		do {
			listView.incrementCurrentIndex();
		} while (currentItem.text === "")
	}
	Keys.onUpPressed: {
		do {
			listView.decrementCurrentIndex();
		} while (currentItem.text === "")
	}

	layer {
		enabled: GraphicsInfo.api !== GraphicsInfo.Software

		effect: GDropShadow {
		}
	}
}
