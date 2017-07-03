import QtQuick 2.5

import "global"

Rectangle {
	id: baseItem

	property string address
	property string providerIcon
	property string providerImage
	property color transparentColor : Constants.blue

	width: parent.width
	height: Utils.dp(160)

	color: Constants.background_color

	Image {
		id: headerBackgroundImage

		anchors.top:  parent.top
		anchors.left:  parent.left
		anchors.right:  parent.right

		width: parent.width
		height: parent.height * 0.7
		source: providerImage
		fillMode: Image.PreserveAspectCrop
	}


	Rectangle {
		width: headerBackgroundImage.width
		height: headerBackgroundImage.height
		color: transparentColor
		opacity: transparentColor === Constants.blue ? 0 : 0.7
	}

	Image {
		anchors.left: baseItem.left
		anchors.leftMargin: Utils.dp(30)
		anchors.verticalCenter: headerBackgroundImage.bottom
		height: Utils.dp(70)
		width: height
		source: providerIcon
		fillMode: Image.Stretch
	}

	Button {
		id: buttonText
		width: baseItem.width / 2
		anchors.bottom: baseItem.bottom
		anchors.right: parent.right
		anchors.rightMargin: Utils.dp(20)
		text: qsTr("Online-Application")
		onClicked: {
			Qt.openUrlExternally(address)
		}
	}
}
