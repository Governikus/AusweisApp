/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Item {
	id: root

	Accessible.role: Accessible.Grouping
	Accessible.name: titleText.text

	property alias title: titleText.text
	property alias titleTextStyle: titleText.textStyle

	readonly property int availableContentHeight: {
		var availableHeight = height - containerCol.topPadding - containerCol.bottomPadding

		if (title === "") {
			return availableHeight
		}

		return availableHeight - titleText.height - containerCol.spacing
	}

	property alias content: paneContent
	default property alias children: paneContent.children

	implicitHeight: containerCol.implicitHeight
	implicitWidth: containerCol.implicitWidth

	Rectangle {
		id: background

		anchors.fill: parent

		color: Style.color.background_pane
		radius: Style.dimens.corner_radius

		border.width: Style.dimens.high_contrast_item_border
		border.color: Style.color.high_contrast_item_border
	}

	Column {
		id: containerCol

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Constants.pane_padding
		anchors.rightMargin: Constants.pane_padding
		topPadding: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing

		GText {
			id: titleText

			width: Math.min(parent.width, implicitWidth)

			textStyle: Style.text.header_accent

			FocusFrame {
				scope: root
				borderColor: Style.color.focus_indicator
			}
		}

		Column {
			id: paneContent

			width: parent.width
			spacing: Constants.pane_spacing
		}
	}

}
