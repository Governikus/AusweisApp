/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

FocusScope {
	id: tile

	property alias image: image.source
	property alias title: text.text

	signal clicked

	Accessible.name: ApplicationModel.stripHtmlTags(title)
	Accessible.role: Accessible.Button

	Keys.onSpacePressed: tile.clicked()

	MouseArea {
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor

		onClicked: tile.clicked()
		onPressed: tile.focus = true
	}
	FocusFrame {
	}
	TintableIcon {
		id: image

		readonly property int imageHeight: Style.dimens.huge_icon_size

		sourceSize.height: imageHeight
		tintColor: text.textStyle.textColor

		anchors {
			bottom: parent.verticalCenter
			bottomMargin: imageHeight * -(1 / 4)
			horizontalCenter: parent.horizontalCenter
		}
	}
	GText {
		id: text
		font.bold: true
		horizontalAlignment: Text.AlignHCenter
		textStyle: Style.text.title_accent

		anchors {
			left: parent.left
			right: parent.right
			top: image.bottom
			topMargin: Constants.component_spacing
		}
	}
}
