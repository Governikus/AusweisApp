import AusweisApp2.Global 1.0

import QtQuick 2.6
import QtQuick.Layouts 1.2


Rectangle {
	id: baseItem
	property var providerModelItem: null
	signal showProviderDetails

	Item {
		id: sectionImage
		height: parent.height
		width: Utils.dp(80)

		Rectangle {
			id: providerImage
			anchors.fill: parent
			anchors.margins: Utils.dp(10)
			color: baseItem.providerModelItem ? Category.displayColor(baseItem.providerModelItem.providerCategory) : Category.displayColor("unknown" )
			visible: !!providerIconSource
			readonly property string providerIconSource: baseItem.providerModelItem ? baseItem.providerModelItem.providerIcon : ""

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
			source: baseItem.providerModelItem ? Category.sectionImageSource(baseItem.providerModelItem.providerCategory) : Category.sectionImageSource("unknown")
			asynchronous: true
			clip: true
			visible: !providerImage.visible
		}
	}
	ProviderCardInfoItem {
		height: parent.height
		anchors.left: sectionImage.right
		anchors.right: parent.right
		providerName: !!providerModelItem.providerLongName ? providerModelItem.providerLongName : providerModelItem.providerShortName
		providerAddressDomain: providerModelItem.providerAddressDomain
	}

	MouseArea {
		anchors.fill: parent
		onDoubleClicked: baseItem.showProviderDetails()
	}
}
