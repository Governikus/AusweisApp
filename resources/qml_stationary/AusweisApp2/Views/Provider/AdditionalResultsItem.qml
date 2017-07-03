import AusweisApp2.Global 1.0

import QtQuick 2.6
import QtQuick.Layouts 1.2


Rectangle {
	id: baseItem
	readonly property int totalHits: providerModel.additionalResultCount

	visible: totalHits > 0 && providerModel.categories.length > 0 && providerModel.categories.indexOf("all") === -1

	Image {
		id: backgroundImage
		anchors.margins: Utils.dp(10)
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		width: Utils.dp(60)
		source: Category.backgroundImageSource("all")
		asynchronous: true
		clip: true
		Image {
			id: foregroundImage
			anchors.fill: parent
			anchors.margins: parent.anchors.margins
			source: Category.imageSource("all")
			asynchronous: true
			clip: true
			fillMode: Image.PreserveAspectFit
		}
	}

	Text {
		text: qsTr("Additional results in other categories: %1").arg(baseItem.totalHits)
		anchors.verticalCenter: parent.verticalCenter
		anchors.left: backgroundImage.right
		anchors.margins: Utils.dp(15)
		anchors.right: parent.right
	}

	MouseArea {
		anchors.fill: parent
		onDoubleClicked: providerModel.addAdditionalResultCategories()
	}
}
