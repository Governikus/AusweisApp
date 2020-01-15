/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0


Rectangle {
	id: baseItem
	property alias imageSource: image.source
	property alias itemText: text.text
	property url link
	property int sizeRecudctor: 0

	height: text.height + 20

	Accessible.role: Accessible.Link
	Accessible.name: ApplicationModel.stripHtmlTags(itemText)

	Image {
		id: image
		width: 25 - baseItem.sizeRecudctor
		height: width
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
	}

	GText {
		id: text

		anchors.left: image.right
		anchors.leftMargin: 10
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter

		linkColor: color
		textStyle: Style.text.normal_inverse
		font.pixelSize: Style.text.normal_inverse.textSize - baseItem.sizeRecudctor
	}

	MouseArea {
		anchors.fill: parent
		enabled: !!baseItem.link
		onClicked: Qt.openUrlExternally(baseItem.link)
	}
}
