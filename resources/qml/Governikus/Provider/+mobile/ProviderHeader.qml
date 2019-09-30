/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0


Rectangle {
	id: baseItem

	// Properties that are set by ProviderView or ProviderDetailView
	property string selectedCategory: selectedProvider ? selectedProvider.category : ""
	property var selectedProvider

	// This is interpreted by the SectionPage component
	readonly property real titleBarOpacity: shadow.opacity === 1 ? 1 : (customProviderImage ? Math.max(0, 0.5 - shadow.opacity) : 0)
	readonly property real titleBarTopBounce: sectionPageFlickable.verticalOvershoot < 0.0 ? -sectionPageFlickable.verticalOvershoot : 0.0

	// Internal vars
	readonly property color shadowColor: Category.displayColor(selectedCategory)
	readonly property bool customProviderImage: !!selectedProvider && !!selectedProvider.image
	readonly property string backgroundImage: customProviderImage ? selectedProvider.image : Category.backgroundImageSource(selectedCategory)
	readonly property string categoryIcon: selectedProvider || selectedCategory !== "all" ? "" : Category.imageSource(selectedCategory)
	readonly property bool withButtons: selectedCategory === "" && !selectedProvider
	readonly property real shadowOpacity: Math.min(1, headerOffsetY / (backgroundImage.height - Style.dimens.titlebar_height))

	readonly property double iconHeightRatio: 0.3
	readonly property double iconVerticalMarginRatio: 0.2

	// For some reason contentY is sometimes set to undefined.
	readonly property real headerOffsetY: typeof(sectionPageFlickable.contentY) === "undefined" ? 0 : sectionPageFlickable.contentY

	property int maxContentY: if (withButtons && parent != null) {
		return parent.height * (iconHeightRatio + iconVerticalMarginRatio)
	} else {
		return height / 2
	}

	height: backgroundImage.height + providerInfo.height - titleBarTopBounce

	color: providerInfo.color // Some provider images have transparent backgrounds, so we need to fill the background

	Image {
		id: backgroundImage

		source: baseItem.backgroundImage
		height: width / 1.80 + titleBarTopBounce
		width: parent.width
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.topMargin: -titleBarTopBounce // When flicking over the top, scale the image (similar to native iOS apps)
		fillMode: Image.PreserveAspectCrop

		Image {
			id: categoryIcon
			source: baseItem.categoryIcon
			asynchronous: true
			height: parent.height * 0.5
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.horizontalCenter: backgroundImage.horizontalCenter
			anchors.bottom: backgroundImage.bottom
			anchors.bottomMargin: Constants.component_spacing

			visible: baseItem.categoryIcon !== ""

			opacity: baseItem.headerOffsetY <= maxContentY ? 1 : 0
			Behavior on opacity {
				NumberAnimation {}
			}
		}

		Image {
			source: selectedProvider ? selectedProvider.icon : ""
			asynchronous: true
			height: 70
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.margins: Constants.component_spacing
			anchors.left: parent.left
			anchors.bottom: parent.bottom
			visible: !!selectedProvider
		}
	}

	Row {
		id: iconsRow

		height: backgroundImage.height * iconHeightRatio
		width: backgroundImage.width * 0.9

		visible: withButtons

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: backgroundImage.bottom
		anchors.bottomMargin: backgroundImage.height / 2 - iconsRow.height - titleBarTopBounce - headerOffsetY / (headerOffsetY < 0.0 ? 1.0 : 2.0)

		Repeater {
			model: ["citizen", "finance", "insurance", "other"]

			Item {
				height: parent.height
				width: parent.width * 0.25

				Image {
					source: Category.buttonImageSource(modelData)
					asynchronous: true
					anchors.fill: parent
					fillMode: Image.PreserveAspectFit
				}

				MouseArea {
					anchors.fill: parent
					onClicked: ProviderCategoryFilterModel.setCategorySelection(modelData)
				}
			}
		}
	}

	Rectangle {
		id: shadow

		anchors.fill: backgroundImage

		color: baseItem.shadowColor
		opacity: shadowOpacity
	}

	Rectangle {
		id: providerInfo
		height: visible ? column.height + 2 * Constants.pane_padding : 0
		width: parent.width
		anchors.left: parent.left
		anchors.top: backgroundImage.bottom

		visible: !!selectedProvider

		Column {
			id: column
			anchors.margins: Constants.pane_padding
			anchors.left: parent.left
			anchors.top: parent.top
			anchors.right: parent.right
			spacing: Constants.pane_spacing

			GText {
				id: providerText
				width: parent.width
				text: selectedProvider ? selectedProvider.shortDescription : ""
				textStyle: Style.text.normal
				visible: text.length > 0
			}

			GButton {
				id: providerButton
				anchors.right: parent.right
				buttonColor: shadowColor
				//: LABEL ANDROID IOS
				text: qsTr("To service provider") + SettingsModel.translationTrigger
				onClicked: {
					Qt.openUrlExternally(selectedProvider ? selectedProvider.address : "")
				}
			}
		}
	}
}
