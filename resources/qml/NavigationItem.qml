import QtQuick 2.5

import "global"

Item {
	property alias source: tabImage.source
	property alias text: tabText.text
	property alias textColor: tabText.color
	signal clicked

	Image {
		id: tabImage
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.topMargin: 5
		anchors.top: parent.top
		anchors.bottom: tabText.top
		fillMode: Image.PreserveAspectFit
	}

	Text {
		id: tabText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: font.pixelSize / 10
		font.pixelSize: Constants.small_font_size
		renderType: Text.NativeRendering
	}

	MouseArea {
		anchors.fill: parent
		onClicked: parent.clicked()
	}
}
