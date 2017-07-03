import QtQuick 2.5

import "global"

Rectangle {
	id: baseItem

	// Properties that are set by HistoryViewPage
	property var selectedProvider

	// Internal vars
	readonly property color shadowColor: Category.displayColor(selectedProvider.category)
	readonly property string backgroundImage: !!selectedProvider.image ? selectedProvider.image : Category.backgroundImageSource(selectedProvider.category)
	readonly property string providerIcon: !!selectedProvider.icon ? selectedProvider.icon : Category.buttonImageSource(selectedProvider.category)
	readonly property string providerAddress: selectedProvider.address
	readonly property string providerShortDescription: selectedProvider.shortDescription

	height: headerBackgroundImage.height + buttonText.height + Constants.pane_spacing
	color: Constants.background_color

	Image {
		id: headerBackgroundImage
		source: backgroundImage
		height: width / 1.80
		width: parent.width
		fillMode: Image.PreserveAspectCrop
		anchors.top:  parent.top
		anchors.left:  parent.left
		anchors.right:  parent.right

		Rectangle {
			anchors.fill: parent
			color: shadowColor
			opacity: shadowColor === Constants.blue ? 0 : 0.7
		}
	}

	Image {
		source: providerIcon
		asynchronous: true
		height: Utils.dp(70)
		width: height
		fillMode: Image.PreserveAspectFit
		anchors.left: baseItem.left
		anchors.leftMargin: Utils.dp(30)
		anchors.verticalCenter: headerBackgroundImage.bottom
	}

	Button {
		id: buttonText
		width: baseItem.width / 2
		anchors.bottom: baseItem.bottom
		anchors.right: parent.right
		anchors.rightMargin: Constants.component_spacing
		text: qsTr("Online Application")
		onClicked: {
			Qt.openUrlExternally(providerAddress)
		}
	}
}
