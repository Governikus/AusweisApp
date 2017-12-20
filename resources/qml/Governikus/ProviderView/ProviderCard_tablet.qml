import QtQuick 2.6
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Provider 1.0

Rectangle {
	id: baseItem
	height: column.height
	color: Category.displayColor(provider.category)

	property int headerHeight: 0
	property int textHeight: 0
	property int footerHeight: 0

	property alias providerModelItem: provider.modelItem
	property var pushFunction: function(model) {}

	ProviderModelItem {
		id: provider
	}

	Column {
		id: column
		width: baseItem.width

		Image {
			source: provider.image
			asynchronous: true
			height: baseItem.headerHeight
			width: parent.width
			fillMode: Image.PreserveAspectCrop
			anchors.horizontalCenter: parent.horizontalCenter
		}

		ProviderCardNameRow_tablet {
			height: baseItem.textHeight
			providerName: provider.longName !== "" ? provider.longName : provider.shortName
			headerIcon: provider.icon
			providerCategory: provider.category
		}

		Rectangle {
			color: Category.displayColor(provider.category)
			height: baseItem.footerHeight
			width: parent.width

			Text {
				text: provider.homepageBase

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
		onClicked: baseItem.pushFunction(providerModelItem)
	}
}
