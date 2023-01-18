/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ProviderCategoryFilterModel 1.0

Rectangle {
	id: baseItem

	readonly property string backgroundImage: customProviderImage ? selectedProvider.image : Category.backgroundImageSource(selectedCategory)
	readonly property bool customProviderImage: !!selectedProvider && !!selectedProvider.image
	readonly property real headerOffsetY: sectionPageFlickable.contentY - sectionPageFlickable.originY
	readonly property double iconHeightRatio: 0.3
	readonly property double iconVerticalMarginRatio: 0.2
	property int maxContentY: if (withButtons && parent !== null) {
		return parent.height * (iconHeightRatio + iconVerticalMarginRatio);
	} else {
		return height / 2;
	}

	// Properties that are set by ProviderView or ProviderDetailView
	property string selectedCategory: selectedProvider ? selectedProvider.category : ""
	property var selectedProvider

	// Internal vars
	readonly property color shadowColor: Category.displayColor(selectedCategory)
	readonly property real shadowOpacity: Math.min(1, headerOffsetY / (backgroundImage.height - Style.dimens.titlebar_height))

	// This is interpreted by the SectionPage component
	readonly property real titleBarOpacity: shadow.opacity === 1 ? 1 : (customProviderImage ? Math.max(0, 0.5 - shadow.opacity) : 0)
	readonly property real titleBarTopBounce: sectionPageFlickable.verticalOvershoot < 0.0 ? -sectionPageFlickable.verticalOvershoot : 0.0
	readonly property bool withButtons: selectedCategory === "" && !selectedProvider

	signal categorySelected(string category)

	color: providerInfo.color
	height: backgroundImage.height + providerInfo.height - titleBarTopBounce

	Image {
		id: backgroundImage
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.topMargin: -titleBarTopBounce // When flicking over the top, scale the image (similar to native iOS apps)
		fillMode: Image.PreserveAspectCrop
		height: width / 1.80 + titleBarTopBounce
		source: baseItem.backgroundImage
		width: parent.width

		Rectangle {
			id: iconContainer
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.margins: Constants.component_spacing
			border.color: Style.color.border
			border.width: Style.dimens.separator_size
			color: Constants.white
			height: 70
			radius: Style.dimens.corner_radius
			visible: !!selectedProvider
			width: height

			Image {
				id: icon
				anchors.fill: parent
				anchors.margins: iconContainer.radius / 2
				asynchronous: true
				fillMode: Image.PreserveAspectFit
				source: selectedProvider ? selectedProvider.icon : ""
			}
		}
	}
	Row {
		id: iconsRow
		anchors.bottom: backgroundImage.bottom
		anchors.bottomMargin: backgroundImage.height / 2 - iconsRow.height - titleBarTopBounce - headerOffsetY / (headerOffsetY < 0.0 ? 1.0 : 2.0)
		anchors.horizontalCenter: parent.horizontalCenter
		height: backgroundImage.height * iconHeightRatio
		visible: withButtons
		width: backgroundImage.width * 0.9

		Repeater {
			model: ["citizen", "finance", "insurance", "other"]

			Item {
				height: parent.height
				width: parent.width * 0.25

				Image {
					Accessible.name: Category.displayString(modelData)
					Accessible.role: Accessible.Button
					anchors.fill: parent
					asynchronous: true
					fillMode: Image.PreserveAspectFit
					source: Category.buttonImageSource(modelData)
				}
				MouseArea {
					anchors.fill: parent

					onClicked: baseItem.categorySelected(modelData)
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
		anchors.left: parent.left
		anchors.top: backgroundImage.bottom
		color: Style.color.background_pane
		height: visible ? column.height + 2 * Constants.pane_padding : 0
		visible: !!selectedProvider
		width: parent.width

		Column {
			id: column
			anchors.left: parent.left
			anchors.margins: Constants.pane_padding
			anchors.right: parent.right
			anchors.top: parent.top
			spacing: Constants.pane_spacing

			GText {
				id: providerText
				text: selectedProvider ? selectedProvider.longName : ""
				visible: text.length > 0
				width: parent.width
			}
			GButton {
				id: providerButton
				anchors.right: parent.right
				buttonColor: shadowColor
				icon.source: "qrc:///images/material_open_in_new.svg"
				//: LABEL ANDROID IOS
				text: qsTr("To provider")
				tintIcon: true

				onClicked: {
					Qt.openUrlExternally(selectedProvider ? selectedProvider.address : "");
				}
			}
		}
	}
}
