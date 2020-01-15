/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Style 1.0

Rectangle {
	id: root
	property alias title: titleText.text
	property alias spacing: paneContent.spacing
	default property alias paneChildren: paneContent.children

	anchors.left: parent.left
	anchors.right: parent.right
	height: content.implicitHeight + 2 * Constants.pane_padding
	color: Style.color.background_pane
	radius: Style.dimens.corner_radius
	border.width: Style.dimens.high_contrast_item_border
	border.color: Style.color.high_contrast_item_border

	Column {
		id: content

		anchors {
		 	fill: parent
			margins: Constants.pane_padding
		}

		spacing: Constants.pane_padding

		PaneTitle {
			id: titleText
			height: implicitHeight * 2
			verticalAlignment: Text.AlignVCenter
		}

		Column {
			id: paneContent
			width: parent.width
			spacing: Constants.pane_spacing
		}
	}
}
