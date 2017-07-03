import QtQuick 2.5

import "global"

Item {
	height: img.height + 2 * img.anchors.topMargin
	width: img.width + text2.anchors.leftMargin + text2.width

	signal clicked

	Rectangle {
		height: 1
		width: parent.width * 1.2
		anchors.horizontalCenter: parent.horizontalCenter
		color: Constants.grey
	}
	Image {
		id: img
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		height: Utils.dp(50)
		fillMode: Image.PreserveAspectFit
		smooth: true
		source: "qrc:///images/icon_Bluetooth.svg"
	}
	Text {
		id: dummyText
		visible: false
		text: qsTr("Bluetooth not supported")
		font.pixelSize: Constants.normal_font_size
	}
	Text {
		id: text1
		anchors.left: img.right
		anchors.leftMargin: Utils.dp(10)
		anchors.bottom: img.verticalCenter
		text: applicationModel.bluetoothEnabled ? qsTr("or via") : qsTr("Bluetooth not supported")
		font.pixelSize: Constants.normal_font_size
		color: applicationModel.bluetoothEnabled ? "black" : Constants.red
	}
	Text {
		id: text2
		anchors.top: img.verticalCenter
		anchors.left: img.right
		anchors.leftMargin: Utils.dp(10)
		width: dummyText.width
		text: applicationModel.bluetoothEnabled ? qsTr("Bluetooth reader") : qsTr("or switched off")
		font.pixelSize: applicationModel.bluetoothEnabled ? Constants.header_font_size : Constants.normal_font_size
		color: applicationModel.bluetoothEnabled ? Constants.blue : Constants.red
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		enabled: applicationModel.bluetoothEnabled
		onClicked: parent.clicked()
	}
}
