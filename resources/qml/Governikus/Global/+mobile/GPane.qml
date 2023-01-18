/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Style 1.0

GPaneBackground {
	id: root

	property alias contentSpacing: paneContent.spacing
	default property alias paneData: paneContent.data
	property alias title: titleText.text

	height: childrenRect.height
	implicitHeight: content.implicitHeight

	Column {
		id: content
		anchors.left: parent.left
		anchors.leftMargin: Constants.pane_padding
		anchors.right: parent.right
		anchors.rightMargin: Constants.pane_padding
		bottomPadding: Constants.pane_padding
		spacing: Constants.pane_spacing
		topPadding: Constants.pane_padding

		PaneTitle {
			id: titleText
			width: parent.width
		}
		Column {
			id: paneContent
			spacing: Constants.pane_spacing
			width: parent.width
		}
	}
}
