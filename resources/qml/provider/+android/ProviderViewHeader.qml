import QtQuick 2.5

import "../global"


Rectangle {
	id: baseItem

	// Properties that are set by ProviderSectionView.
	property string selectedCategory: ""

	property bool withButtons: true

	/* This is interpreted by the SectionPage component. */
	readonly property real titleBarOpacity: shadow.opacity === 1 ? 1 : 0
	readonly property int stopFlickOn: height - Constants.titlebar_height

	property color shadowColor: selectedCategory === "" ? Constants.blue : Category.displayColor(selectedCategory)

	property string foregroundImageSource: selectedCategory === "all" ? "qrc:///images/All.png" : ""

	property string backgroundImageSource: selectedCategory === "" ? "qrc:///images/provider/categoryIcons/+android/General_bg.png" :
																	 Category.backgroundImageSource(selectedCategory)

	readonly property double iconHeightRatio: 0.3

	readonly property double iconVerticalMarginRatio: 0.2

	property int maxContentY: withButtons ? parent.height * (iconHeightRatio + iconVerticalMarginRatio) :
											height / 2

	readonly property string searchText: ""

	ProviderStyle {
		id: providerStyle
	}

	height: providerStyle.providerHeaderHeight

	function definedContentY() {
		// For some reason contentY is sometimes set to undefined.
		return typeof(contentY) === "undefined" ? 0 : contentY
	}

	function currentMargin() {
		// Height of button icons.
		var H = height * iconHeightRatio

		// Initial inferior margin for button icons.
		var M = height * iconVerticalMarginRatio

		var y = definedContentY()

		return -2 * y * (M + H) / (3 * M  + 2 * H) + M
	}

	Image {
		id: backgroundImage

		width: parent.width
		source: baseItem.backgroundImageSource
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		Rectangle {
			id: shadow
			anchors.fill: parent
			color: baseItem.shadowColor

			opacity: baseItem.definedContentY() <= maxContentY ? 0.5 : 1
			Behavior on opacity {
				NumberAnimation {}
			}
		}

		Rectangle {
			id: iconBackground

			anchors.centerIn: icon
			height: icon.height
			width: height
			radius: width * 0.5

			border.width: 0
			color: Constants.all_image_background_color

			visible: baseItem.foregroundImageSource !== ""

			opacity: baseItem.definedContentY() <= maxContentY ? 1 : 0
			Behavior on opacity {
				NumberAnimation {}
			}
		}

		Image {
			id: icon
			source: baseItem.foregroundImageSource

			anchors.horizontalCenter: backgroundImage.horizontalCenter
			anchors.bottom: backgroundImage.bottom
			anchors.bottomMargin: Utils.dp(20)

			fillMode: Image.PreserveAspectFit
			width: baseItem.width * 0.26
			height: width

			visible: baseItem.foregroundImageSource !== ""

			opacity: baseItem.definedContentY() <= maxContentY ? 1 : 0
			Behavior on opacity {
				NumberAnimation {}
			}
		}
	}

	Row {
		id: iconsRow

		height: parent.height * iconHeightRatio
		width: parent.width * 0.9

		visible: withButtons

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		anchors.bottomMargin: baseItem.currentMargin()

		Repeater {
			model: ["citizen", "insurance", "finance", "other"]

			Rectangle {
				height: parent.height
				width: parent.width * 0.25
				color: "transparent"

				Image {
					source: Category.buttonImageSource(modelData)
					opacity: 1.0

					anchors.fill: parent

					fillMode: Image.PreserveAspectFit
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						push(categoryProviderView, {selectedCategory: modelData})
					}
				}
			}
		}
	}
}
