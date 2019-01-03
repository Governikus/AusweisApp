import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

Rectangle {
	property string sectionName: ""

	width: parent.width
	height: Constants.provider_section_height
	clip: true

	Item {
		anchors.fill: parent
		anchors.topMargin: Utils.dp(5)
		anchors.bottomMargin: Utils.dp(5)

		Image {
			id: sectionImage
			source: Category.imageSource(sectionName)
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
			anchors.left: sectionImage.right
			anchors.leftMargin: ProviderStyle.leftProviderListMargin
			color: ProviderStyle.categoryColor
			font.pixelSize: ProviderStyle.categoryFontPixelSize
			font.bold: ProviderStyle.categoryFontBold
			elide: Text.ElideRight
			text: Category.displayString(sectionName) + settingsModel.translationTrigger
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
			onClicked: {
				ProviderCategoryFilterModel.setCategorySelection(sectionName)
			}
		}
	}

	Rectangle {
		width: parent.width * 0.85
		anchors.top: parent.bottom
		anchors.topMargin: -height
		anchors.right: parent.right
		height: 1
		color: Constants.grey
	}
}
