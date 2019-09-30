/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0

import "Utils.js" as Utils

Column {
	id: root
	property alias title: titleText.text
	property alias contentSpacing: paneContent.spacing
	default property alias paneData: paneContent.data

	anchors.left: parent.left
	anchors.right: parent.right

	spacing: Constants.groupbox_spacing

	PaneTitle {
		id: titleText
		anchors.margins: Constants.pane_padding
		font.capitalization: Font.AllUppercase
	}

	Rectangle {
		color: Style.color.background_pane
		width: parent.width
		height: paneContent.height + 2 * Constants.pane_padding
		radius: Style.dimens.corner_radius

		Column {
			id: paneContent
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.margins: Constants.pane_padding
			spacing: Constants.component_spacing
		}
	}
}
