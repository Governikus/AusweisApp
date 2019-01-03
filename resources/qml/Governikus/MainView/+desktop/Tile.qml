import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0

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
	}

	FocusFrame {
		marginFactor: -2
	}

	Column {
		anchors.centerIn: parent
		spacing: text.height / 2

		Image {
			id: image
			sourceSize.height: text.height * 4
			anchors.horizontalCenter: parent.horizontalCenter
		}

		GText {
			id: text
			anchors.horizontalCenter: parent.horizontalCenter
			color: Constants.white
			font.pixelSize: Constants.header_font_size
			font.weight: Font.Bold
		}
	}
}
