import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "../"
import "../global"


Item {
    property alias text: textItem.text
    property alias imageSource: imageItem.source
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
		height: parent.height
		verticalAlignment: Text.AlignVCenter
		anchors.left: imageItem.right
		anchors.leftMargin: Utils.dp(10)
		anchors.verticalCenter: parent.verticalCenter
		font.pixelSize: Utils.dp(16)
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

