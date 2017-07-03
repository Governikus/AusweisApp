import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../global"

Rectangle {
	id: baseItem
	height: Constants.provider_section_height

	property int totalHits: providerModel.additionalResultCount

	visible: totalHits > 0 && providerModel.categories.length > 0 && providerModel.categories.indexOf("all") === -1

	Item {
		anchors.fill: parent
		anchors.topMargin: Utils.dp(5)
		anchors.bottomMargin: Utils.dp(5)

		Image {
			id: allImage
			source: Category.imageSource("all")
			asynchronous: true
			height: parent.height
			width: parent.width * 0.15
			fillMode: Image.PreserveAspectFit
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter
			anchors.leftMargin: providerStyle.leftIconMargin
		}

		Text {
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: allImage.right
			anchors.leftMargin: providerStyle.leftProviderListMargin
			color: providerStyle.categoryColor
			font.pixelSize: providerStyle.categoryFontPixelSize
			font.bold: providerStyle.categoryFontBold
			elide: Text.ElideRight
			text: '<html>' + qsTr("Additional results:") + "&nbsp;" + baseItem.totalHits + '</html>'
		}

		Text {
			anchors.right: parent.right
			anchors.rightMargin: Utils.dp(5)
			anchors.verticalCenter: parent.verticalCenter

			text: ">"
			color: Constants.grey
			font.pixelSize: Constants.normal_font_size
			visible: providerStyle.showCategoryRightArrow
		}

		MouseArea {
			anchors.fill: parent
			onClicked: providerModel.setCategorySelection("")
		}
	}
}
