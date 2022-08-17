/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

FocusScope {
	id: tile

	property alias title: text.text
	property alias image: image.source

	signal clicked()

	Accessible.role: Accessible.Button
	Accessible.name: ApplicationModel.stripHtmlTags(title)

	Keys.onSpacePressed: tile.clicked()

	MouseArea {
		anchors.fill: parent
		onPressed: tile.focus = true
		onClicked: tile.clicked()
		cursorShape: Qt.PointingHandCursor
	}

	FocusFrame {}

	TintableIcon {
		id: image

		readonly property int imageHeight: Style.dimens.huge_icon_size

		anchors {
			bottom: parent.verticalCenter
			horizontalCenter: parent.horizontalCenter
			bottomMargin: imageHeight * -(1/4)
		}

		sourceSize.height: imageHeight

		tintColor: text.textStyle.textColor
	}

	GText {
		id: text

		anchors {
			left: parent.left
			right: parent.right
			top: image.bottom
			topMargin: Constants.component_spacing
		}

		textStyle: Style.text.title_accent
		font.bold: true
		horizontalAlignment: Text.AlignHCenter
	}
}
