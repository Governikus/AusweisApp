/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
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

	Accessible.role: Accessible.ListItem
	Accessible.name: nameRow.providerName
	Accessible.onPressAction: if (Qt.platform.os === "ios") mouseArea.clicked(null)

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

		ProviderCardNameRow {
			id: nameRow

			height: baseItem.textHeight

			providerName: provider.longName !== "" ? provider.longName : provider.shortName
			headerIcon: provider.icon
			providerCategory: provider.category
		}

		Rectangle {
			color: Category.displayColor(provider.category)
			height: baseItem.footerHeight
			width: parent.width
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.pushFunction(providerModelItem)
	}
}
