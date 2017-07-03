import QtQuick 2.5

import "../global"

Item {
	id: baseItem

	property string imageUrl: ""

	readonly property int imageMargin: Utils.dp(10)

	anchors.left: parent.left
	anchors.top: parent.top
	anchors.bottom: parent.bottom

	height: parent.height
	width: Utils.dp(80)

	Rectangle {
		id: background
		anchors.centerIn: parent
		height: parent.height - 2 * parent.imageMargin
		width: parent.width - 2 * parent.imageMargin
		color:  historyModelItem ? Category.displayColor(historyModelItem.providerCategory) : Category.displayColor("unknown" )

		visible: baseItem.imageUrl !== ""
	}

	Image {
		id: categoryImage
		anchors.centerIn: parent
		height: parent.height - 2 * parent.imageMargin
		width: parent.width - 2 * parent.imageMargin
		source: historyModelItem ? Category.sectionImageSource(historyModelItem.providerCategory) : Category.sectionImageSource("unknown")
		asynchronous: true
		clip: true

		visible: baseItem.imageUrl === ""
	}

	Image {
		id: foregroundImage
		source: baseItem.imageUrl
		anchors.fill: background
		anchors.margins: parent.imageMargin
		asynchronous: true

		visible: baseItem.imageUrl !== ""
		fillMode: Image.PreserveAspectFit
	}
}
