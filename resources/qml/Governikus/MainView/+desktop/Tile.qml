/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

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
	Accessible.name: title

	Keys.onSpacePressed: tile.clicked()

	MouseArea {
		anchors.fill: parent
		onPressed: tile.focus = true
		onClicked: tile.clicked()
		cursorShape: Qt.PointingHandCursor
	}

	FocusFrame {
		marginFactor: -2
	}

	Column {
		anchors.centerIn: parent

		spacing: Constants.component_spacing

		Image {
			id: image

			sourceSize.height: ApplicationModel.scaleFactor * 200
			anchors.horizontalCenter: parent.horizontalCenter
		}

		GText {
			id: text

			anchors.horizontalCenter: parent.horizontalCenter

			textStyle: Style.text.title
			font.bold: true
		}
	}
}
