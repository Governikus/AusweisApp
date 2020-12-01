/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Style 1.0

GPaneBackground {
	id: root
	property alias title: titleText.text
	property alias contentSpacing: paneContent.spacing
	default property alias paneChildren: paneContent.children

	height: childrenRect.height

	implicitHeight: content.implicitHeight

	Column {
		id: content

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
}
