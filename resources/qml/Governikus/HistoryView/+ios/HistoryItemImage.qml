import QtQuick 2.5

import Governikus.Global 1.0

Item {
	property string imageUrl: ""

	id: baseItem

	anchors.left: parent.left
	anchors.leftMargin: Utils.dp(10)

	height: parent.height
	width: Utils.dp(40)

	Image {
		source: baseItem.imageUrl !== "" ?
				baseItem.imageUrl :
				(historyModelItem ? Category.imageSource(historyModelItem.providerCategory) : Category.imageSource("unknown"))
		asynchronous: true
		height: Math.min(parent.height * 0.6, parent.width)
		width: height
		fillMode: Image.PreserveAspectFit
		anchors.centerIn: parent
	}
}
