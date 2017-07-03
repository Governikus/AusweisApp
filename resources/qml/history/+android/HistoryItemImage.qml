import QtQuick 2.5

import "../global"

Item {
	property string imageUrl: ""

	id: baseItem

	anchors.left: parent.left
	anchors.top: parent.top
	anchors.bottom: parent.bottom

	height: parent.height
	width: Utils.dp(60)


	Image {
		id: backgroundImage
		anchors.centerIn: parent
		height: parent.height
		width: parent.width
		source: modelItem ? Category.sectionImageSource(modelItem.provider_category) : Category.sectionImageSource("unknown")
		clip: true
	}

	Image {
		id: foregroundImage
		source: baseItem.imageUrl
		anchors.verticalCenter: backgroundImage.verticalCenter
		anchors.left: backgroundImage.left
		anchors.right: backgroundImage.right
		anchors.leftMargin: Utils.dp(5)
		anchors.rightMargin: Utils.dp(5)
		visible: baseItem.imageUrl !== ""
		fillMode: Image.PreserveAspectFit
	}
}
