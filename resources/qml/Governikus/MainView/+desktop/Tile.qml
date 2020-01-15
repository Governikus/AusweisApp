/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
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
		dynamic: true
	}

	Column {
		anchors.centerIn: parent

		spacing: Constants.component_spacing

		TintableIcon {
			id: image

			sourceSize.height: ApplicationModel.scaleFactor * 200
			anchors.horizontalCenter: parent.horizontalCenter

			tintColor: text.textStyle.textColor
		}

		GText {
			id: text

			anchors.horizontalCenter: parent.horizontalCenter

			textStyle: Style.text.title_inverse
			font.bold: true
		}
	}
}
