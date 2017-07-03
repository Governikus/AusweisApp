import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../../../global"

Rectangle {
	id: baseItem
	height: column.height
	color: Category.displayColor(providerModel.providerCategory)

	property int headerHeight: 0
	property int textHeight: 0
	property int footerHeight: 0

	property var providerModel: null
	property var pushFunction: function(model) {}

	Column {
		id: column
		width: baseItem.width

		Image {
			source: providerModel.providerImage !== "" ? providerModel.providerImage :
					Category.backgroundImageSource(providerModel.providerCategory)
			asynchronous: true
			height: baseItem.headerHeight
			width: parent.width
			fillMode: Image.PreserveAspectCrop
			anchors.horizontalCenter: parent.horizontalCenter
		}

		ProviderCardNameRow {
			height: baseItem.textHeight
			providerName: providerModel.providerLongName !== "" ? providerModel.providerLongName : providerModel.providerShortName
			headerIcon:  providerModel.providerIcon
			providerCategory: providerModel.providerCategory
		}

		Rectangle {
			color: Category.displayColor(providerModel.providerCategory)
			height: baseItem.footerHeight
			width: parent.width

			Text {
				text: providerModel.providerHomepageBase

				anchors.centerIn: parent

				leftPadding: Constants.pane_padding
				rightPadding: Constants.pane_padding
				elide: Text.ElideRight
				maximumLineCount: 1

				font.pixelSize: Constants.normal_font_size
				color: "white"

				scale: Math.min(1, parent.width / (contentWidth + leftPadding + rightPadding))
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: baseItem.pushFunction(providerModel)
	}
}
