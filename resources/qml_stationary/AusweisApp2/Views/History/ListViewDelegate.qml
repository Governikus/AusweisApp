import AusweisApp2.Global 1.0

import QtQuick 2.6
import QtQuick.Layouts 1.2


Rectangle {
	id: baseItem
	property var historyModelItem
	signal showHistoryDetails

	Item {
		id: sectionImage
		height: parent.height
		width: Utils.dp(80)

		Rectangle {
			id: providerImage
			anchors.fill: parent
			anchors.margins: Utils.dp(10)
			color: baseItem.historyModelItem ? Category.displayColor(baseItem.historyModelItem.providerCategory) : Category.displayColor("unknown" )
			visible: !!providerIconSource
			readonly property string providerIconSource: baseItem.historyModelItem ? baseItem.historyModelItem.providerIcon : ""

			Image {
				source: parent.providerIconSource
				anchors.fill: parent
				anchors.margins: Utils.dp(10)
				asynchronous: true
				fillMode: Image.PreserveAspectFit
			}
		}
		Image {
			id: categoryImage
			anchors.fill: parent
			anchors.margins: Utils.dp(10)
			source: baseItem.historyModelItem ? Category.sectionImageSource(baseItem.historyModelItem.providerCategory) : Category.sectionImageSource("unknown")
			asynchronous: true
			clip: true
			visible: !providerImage.visible
		}
	}

	ListViewDelegateContent {
		anchors.left: sectionImage.right
		anchors.right: parent.right
		height: parent.height
		historyModelItem: baseItem.historyModelItem
	}

	MouseArea {
		anchors.fill: parent
		onDoubleClicked: baseItem.showHistoryDetails()
	}
}
