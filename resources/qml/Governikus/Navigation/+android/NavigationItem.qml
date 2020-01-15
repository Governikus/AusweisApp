/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	property alias source: tabImage.source
	property alias text: tabText.text
	signal clicked

	Accessible.role: Accessible.MenuItem
	Accessible.name: text

	Item {
		id: tabImageItem
		height: parent.height
		width: Style.dimens.menubar_width
		anchors.left: parent.left

		TintableIcon {
			id: tabImage
			height: 35
			sourceSize.width: height
			fillMode: Image.PreserveAspectFit
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			tintColor: Style.color.menu_icon_tint_color
		}
	}

	GText {
		id: tabText

		anchors.left: tabImageItem.right
		anchors.verticalCenter: parent.verticalCenter

		Accessible.ignored: true

		textStyle: Style.text.hint
	}

	MouseArea {
		anchors.fill: parent
		onClicked: parent.clicked()
	}
}
