/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Style 1.0

GPaneBackground {
	id: root
	property alias title: titleText.text
	property alias contentSpacing: paneContent.spacing
	default property alias paneData: paneContent.data

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

			width: parent.width
		}

		Column {
			id: paneContent
			width: parent.width
			spacing: Constants.pane_spacing
		}
	}
}
