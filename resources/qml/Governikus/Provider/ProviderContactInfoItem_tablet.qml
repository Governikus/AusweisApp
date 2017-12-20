import QtQuick 2.6

import Governikus.Global 1.0


Rectangle {
	id: baseItem
	property alias imageSource: image.source
	property alias itemText: text.text
	property url link
	property int sizeRecudctor: 0
	height: text.height + Utils.dp(20)


	Image {
		id: image
		width: Utils.dp(25) - baseItem.sizeRecudctor
		height: width
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter
		fillMode: Image.PreserveAspectFit
	}

	Text {
		id: text
		anchors.left: image.right
		anchors.leftMargin: Utils.dp(10)
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		font.pixelSize: Constants.normal_font_size - baseItem.sizeRecudctor
		color: "white"
		linkColor: color
		elide: Text.ElideRight
		wrapMode: Text.WordWrap
    }

	MouseArea {
		anchors.fill: parent
		enabled: !!baseItem.link
		onClicked: Qt.openUrlExternally(baseItem.link)
	}
}
