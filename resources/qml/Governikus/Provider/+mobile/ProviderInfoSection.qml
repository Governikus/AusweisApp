import QtQuick 2.10

import Governikus.Global 1.0


Rectangle {

	property string imageSource;
	property string title;
	property string name;

	width: parent.width
	height: Math.max(image.height, providerTitle.height)

	color: "white"
	clip: true

	Image {
		id: image
		anchors.left: parent.left
		anchors.verticalCenter: parent.verticalCenter

		height: Utils.dp(40)
		width: Utils.dp(40)

		source: imageSource
		fillMode: Image.PreserveAspectFit
	}

	LabeledText {
		id: providerTitle
		anchors.verticalCenter: image.verticalCenter
		anchors.left: image.right
		anchors.leftMargin: Utils.dp(10)
		anchors.right: parent.right

		label: title
		text: name.length > 0 ? name : qsTr("Touch for more details") + settingsModel.translationTrigger
	}
}
