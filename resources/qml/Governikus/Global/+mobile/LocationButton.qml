import QtQuick 2.10

import "Utils.js" as Utils

MouseArea {
	property string language
	property string name
	property string image

	height: Utils.dp(35)
	width: height

	onClicked: {
		settingsModel.language = language
		if (typeof(navigationController) !== "undefined") {
			navigationController.close()
		}
	}

	Rectangle {
		opacity: 0.1
		border.color: "black"
		border.width: settingsModel.language === language ? 0 : Utils.dp(1)
		color: settingsModel.language === language ? "black" : Constants.background_color
		anchors.fill: parent
		radius: Utils.dp(3)
	}

	Text {
		text: name
		color: Constants.secondary_text

		anchors.margins: Utils.dp(2)
		anchors.top: parent.top
		anchors.horizontalCenter: parent.horizontalCenter
		font.pixelSize: Constants.small_font_size
	}

	Image {
		source: image
		fillMode: Image.PreserveAspectFit

		anchors.margins: Utils.dp(4)
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right
	}
}
