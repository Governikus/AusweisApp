import QtQuick 2.10
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

Rectangle {
	id: baseItem
	height: Constants.provider_section_height

	property int totalHits: ProviderCategoryFilterModel.additionalResultCount

	visible: totalHits > 0 && ProviderCategoryFilterModel.categories.length > 0 && ProviderCategoryFilterModel.categories.indexOf("all") === -1

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
			anchors.leftMargin: ProviderStyle.leftIconMargin
		}

		Text {
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: allImage.right
			anchors.leftMargin: ProviderStyle.leftProviderListMargin
			color: ProviderStyle.categoryColor
			font.pixelSize: ProviderStyle.categoryFontPixelSize
			font.bold: ProviderStyle.categoryFontBold
			elide: Text.ElideRight
			text: '<html>' + qsTr("Additional results:") + "&nbsp;" + baseItem.totalHits + '</html>' + settingsModel.translationTrigger
		}

		Text {
			anchors.right: parent.right
			anchors.rightMargin: Utils.dp(5)
			anchors.verticalCenter: parent.verticalCenter

			text: ">"
			color: Constants.grey
			font.pixelSize: Constants.normal_font_size
			visible: ProviderStyle.showCategoryRightArrow
		}

		MouseArea {
			anchors.fill: parent
			onClicked: ProviderCategoryFilterModel.setCategorySelection("")
		}
	}
}
