import QtQuick 2.6

import AusweisApp2.Global 1.0


Rectangle {
	id: baseItem
	property alias imageSource: image.source
	property alias itemText: text.text
	property url link
	height: Math.max(Utils.dp(60), text.height + Utils.dp(10))

	Item {
		id: iconItem
		height: parent.height
		width: Math.min(height, Utils.dp(60))
		anchors.left: parent.left

		Image {
			id: image
			width: parent.width * 0.5
			height: width
			anchors.centerIn: parent
			fillMode: Image.PreserveAspectFit
		}
	}
	Text {
		id: text
		anchors.left: iconItem.right
		anchors.leftMargin: Utils.dp(10)
		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		font.pixelSize: Constants.normal_font_size
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
