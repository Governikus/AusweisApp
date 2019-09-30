/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Style 1.0

Rectangle {
	id: root
	property alias title: titleText.text
	default property alias paneChildren: paneContent.children

	anchors.left: parent.left
	anchors.right: parent.right
	height: childrenRect.height
	color: Style.color.background_pane
	radius: 16

	Column {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.leftMargin: Constants.pane_padding
		anchors.rightMargin: Constants.pane_padding
		topPadding: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing

		PaneTitle {
			id: titleText
		}

		Column {
			id: paneContent
			width: parent.width
			spacing: Constants.pane_spacing
		}
	}

	layer.enabled: true
	layer.effect: DropShadow {
					radius: 8
					samples: 8
					source: root
					color: Constants.grey
					verticalOffset: 2
				}
}
