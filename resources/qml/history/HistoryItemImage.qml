import QtQuick 2.5

import "../global"

Item {
	property string imageUrl: ""

	id: baseItem

	anchors.left: parent.left
	anchors.leftMargin: Utils.dp(10)

	height: parent.height
	width: Utils.dp(40)

	Image {
		anchors.centerIn: parent
		height: parent.height * 0.6
		width: height
		source: baseItem.imageUrl !== "" ?
				baseItem.imageUrl :
				(modelItem ? Category.imageSource(modelItem.provider_category) : Category.imageSource("unknown"))

		fillMode: Image.PreserveAspectFit
	}
}
