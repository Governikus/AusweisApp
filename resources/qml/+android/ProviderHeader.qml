import QtQuick 2.5

import "global"

Rectangle {
	id: baseItem

	// Properties that are set by ProviderView or ProviderDetailView
	property string selectedCategory: selectedProvider ? selectedProvider.category : ""
	property var selectedProvider

	// This is interpreted by the SectionPage component
	readonly property real titleBarOpacity: shadow.opacity === 1 ? 1 : (customProviderImage ? Math.max(0, 0.5 - shadow.opacity) : 0)

	// Internal vars
	readonly property color shadowColor: Category.displayColor(selectedCategory)
	readonly property bool customProviderImage: !!selectedProvider && !!selectedProvider.image
	readonly property string backgroundImage: customProviderImage ? selectedProvider.image : Category.backgroundImageSource(selectedCategory)
	readonly property string categoryIcon: selectedProvider || selectedCategory !== "all" ? "" : Category.imageSource(selectedCategory)
	readonly property bool withButtons: selectedCategory === "" && !selectedProvider

	readonly property double iconHeightRatio: 0.3
	readonly property double iconVerticalMarginRatio: 0.2

	property int maxContentY: withButtons ? parent.height * (iconHeightRatio + iconVerticalMarginRatio) :
											height / 2

	height: backgroundImage.height + providerInfo.height

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

		source: baseItem.backgroundImage
		height: width / 1.80
		width: parent.width
		anchors.left: parent.left
		anchors.top: parent.top
		fillMode: Image.PreserveAspectCrop

		function transition() {
			return Math.min(1, contentY / (height - Constants.titlebar_height))
		}

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

			opacity: baseItem.definedContentY() <= maxContentY ? 1 : 0
			Behavior on opacity {
				NumberAnimation {}
			}
		}

		Image {
			source: selectedProvider ? selectedProvider.icon : ""
			asynchronous: true
			height: Utils.dp(70)
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.margins: Constants.component_spacing
			anchors.left: parent.left
			anchors.bottom: parent.bottom
			visible: !!selectedProvider
		}

		Rectangle {
			id: shadow
			anchors.fill: parent
			color: baseItem.shadowColor
			opacity: parent.transition()
		}
	}

	Row {
		id: iconsRow

		height: backgroundImage.height * iconHeightRatio
		width: backgroundImage.width * 0.9

		visible: withButtons

		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: backgroundImage.bottom
		anchors.bottomMargin: baseItem.currentMargin()

		Repeater {
			model: ["citizen", "finance", "insurance", "other"]

			Rectangle {
				height: parent.height
				width: parent.width * 0.25
				color: "transparent"

				Image {
					source: Category.buttonImageSource(modelData)
					asynchronous: true
					anchors.fill: parent
					fillMode: Image.PreserveAspectFit
				}

				MouseArea {
					anchors.fill: parent
					onClicked: providerModel.setCategorySelection(modelData)
				}
			}
		}
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

			Text {
				id: providerText
				width: parent.width
				text: selectedProvider ? selectedProvider.shortDescription : ""
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
				visible: text.length > 0
			}

			Button {
				id: providerButton
				anchors.right: parent.right
				buttonColor: shadowColor
				text: qsTr("Online Application")
				onClicked: {
					Qt.openUrlExternally(selectedProvider ? selectedProvider.address : "")
				}
			}
		}
	}
}
