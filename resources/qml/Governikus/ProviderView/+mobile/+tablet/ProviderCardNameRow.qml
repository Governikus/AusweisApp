/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0


Rectangle {
	readonly property int padding: Constants.pane_padding / 2

	property string providerName
	property string headerIcon
	property int nameHeight
	property string providerCategory

	width: parent.width

	Image {
		id: image
		source: parent.headerIcon !== "" ?
				parent.headerIcon : (parent.providerCategory !== "" ?
				Category.buttonImageSource(parent.providerCategory) : Category.buttonImageSource("all"))
		asynchronous: true
		height: parent.height
		width: height
		fillMode: Image.PreserveAspectFit
		anchors.top: parent.top
		anchors.topMargin: -parent.padding
		anchors.left: parent.left
		anchors.leftMargin: parent.padding
	}

	GText {
		anchors.left: image.right
		anchors.leftMargin: parent.padding
		anchors.top: parent.top
		anchors.topMargin: parent.height * 0.05
		anchors.right: parent.right
		anchors.rightMargin: parent.padding

		Accessible.ignored: true

		text: '<html>' + providerName + '</html>'
		elide: Text.ElideRight
		maximumLineCount: 4
		textStyle: Style.text.hint_secondary
	}
}
