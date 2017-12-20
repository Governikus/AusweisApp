import QtQuick 2.7

import Governikus.Global 1.0

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
			anchors.leftMargin: providerStyle.leftIconMargin
		}

		Text {
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: sectionImage.right
			anchors.leftMargin: providerStyle.leftProviderListMargin
			color: providerStyle.categoryColor
			font.pixelSize: providerStyle.categoryFontPixelSize
			font.bold: providerStyle.categoryFontBold
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
			visible: providerStyle.showCategoryRightArrow
		}

		MouseArea {
			anchors.fill: parent
			onClicked: {
				providerModel.setCategorySelection(sectionName)
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
