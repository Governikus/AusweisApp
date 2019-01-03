import QtQuick 2.10

import Governikus.Global 1.0


Item {
	property alias imageSource: imageItem.source
	property alias text: textItem.text
	property bool showRightArrow: false
	signal clicked
	height: Utils.dp(40)
	width: parent.width

	Image {
		id: imageItem
		fillMode: Image.PreserveAspectFit
		height: parent.height * 0.6
		width: height
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(15)
		anchors.verticalCenter: parent.verticalCenter
	}

	Text {
		id: textItem
		color: Constants.secondary_text
		height: parent.height
		verticalAlignment: Text.AlignVCenter
		anchors.left: imageItem.right
		anchors.leftMargin: Utils.dp(10)
		anchors.verticalCenter: parent.verticalCenter
		font.pixelSize: Utils.dp(16)
	}

	Text {
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(5)
		anchors.verticalCenter: parent.verticalCenter

		text: ">"
		color: Constants.grey
		font.pixelSize: Constants.normal_font_size
		visible: parent.showRightArrow
	}

	MouseArea {
		anchors.fill: parent
		onClicked: parent.clicked()
	}

	Rectangle {
		anchors.bottom: parent.bottom
		anchors.left: textItem.left
		anchors.right: parent.right
		height: 1
		color: Constants.grey
	}
}

