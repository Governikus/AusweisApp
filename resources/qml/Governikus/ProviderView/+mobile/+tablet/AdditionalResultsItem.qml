/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0


Rectangle {
	id: baseItem
	height: column.height

	property int headerHeight: 0
	property int textHeight: 0
	property int footerHeight: 0
	property int totalHits: ProviderCategoryFilterModel.additionalResultCount

	visible: totalHits > 0

	Accessible.role: Accessible.Button
	Accessible.name: qsTr("%1 additional results in other categories").arg(totalHits) + SettingsModel.translationTrigger
	Accessible.description: qsTr("Click to remove category filter and show additional results.") + SettingsModel.translationTrigger
	Accessible.onPressAction: if (Qt.platform.os === "ios") mouseArea.clicked(null)

	Column {
		id: column
		width: baseItem.width

		Image {
			id: backgroundImage
			source: Category.backgroundImageSource("all")
			asynchronous: true
			height: baseItem.headerHeight
			width: parent.width
			fillMode: Image.PreserveAspectCrop
			anchors.horizontalCenter: parent.horizontalCenter
		}

		Rectangle {
			id: textRectangle
			height: baseItem.textHeight
			width: parent.width

			GText {
				anchors.centerIn: parent

				Accessible.ignored: true

				//: LABEL ANDROID_TABLET IOS_TABLET
				text: '<html>' + qsTr("Additional results:") + "&nbsp;" + baseItem.totalHits + '</html>' + SettingsModel.translationTrigger
			}
		}

		Rectangle {
			height: baseItem.footerHeight
			width: parent.width
			color: Constants.blue
		}
	}

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: ProviderCategoryFilterModel.addAdditionalResultCategories()
	}
}
