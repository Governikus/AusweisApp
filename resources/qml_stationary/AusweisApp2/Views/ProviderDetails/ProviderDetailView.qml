import AusweisApp2.Global 1.0

import QtQuick 2.6
import QtQuick.Controls 1.4


Item {
	id: baseItem

	property alias providerModelItem: provider.modelItem

	ProviderModelItem {
		id: provider
	}

	function historyInfoIsOpen() {
		return providerDetailsHistoryInfo.visible
	}

	function closeHistoryInfo() {
		providerDetailsHistoryInfo.visible = false
	}

	property color titleBarColor: Category.displayColor(provider.category)


	Rectangle {
		id: titleBar
		height: Utils.dp(40)
		width: baseItem.width
		color: baseItem.titleBarColor

		Text {
			text: provider.shortName
			font.bold: true
			color: "white"

			anchors.left: titleBar.left
			anchors.leftMargin: Constants.component_spacing
			anchors.verticalCenter: titleBar.verticalCenter
		}
	}

	Row {
		id: header
		height: Utils.dp(300)
		width: parent.width
		anchors.top: titleBar.bottom

		Item {
			height: parent.height
			width: baseItem.width * 2 / 3
			anchors.top: parent.top

			Image {
				id: image
				source: provider.image !== "" ? provider.image :
						Category.backgroundImageSource(provider.category)
				asynchronous: true
				height: parent.height
				fillMode: Image.PreserveAspectFit
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
			}

			Image {
				height: parent.height
				width: height / 2
				anchors.right: image.right
				anchors.top: parent.top
				fillMode: Image.Stretch
				source: Category.gradientImageSource(provider.category)

			}

			Rectangle {
				anchors.left: image.right
				anchors.right: parent.right
				anchors.top: parent.top
				height: parent.height
				color: baseItem.titleBarColor
			}
		}

		Rectangle {
			height: parent.height
			width: baseItem.width / 3
			color: baseItem.titleBarColor

			ProviderContactInfo {
				color: baseItem.titleBarColor
				height: parent.height
				width: baseItem.width / 3 - Constants.component_spacing
				contactModel: provider.contactModel
			}
		}
	}

	ProviderDetailButtonBar {
		id: buttonBar
		selectedCategory: provider.category
		providerIcon: provider.icon
		address: provider.address
		titleBarColor: baseItem.titleBarColor
		anchors.top: header.bottom
	}

	Pane {
		id: leftPane
		anchors.margins: Constants.component_spacing
		anchors.top: buttonBar.bottom
		anchors.bottom: closeButton.top

		ProviderDetailDescription {
			id: descriptionData
			width: parent.width
			height: leftPane.height - 2 * Constants.pane_padding
			description: provider.longDescription
		}
	}

	Button {
		id: closeButton
		text: "Close"
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Constants.component_spacing
		onClicked: close()
	}

	property var openHistoryInfoFunc: function(entryInfo) {
		providerDetailsHistoryInfo.visible = true

		providerDetailsHistoryInfo.providerName = entryInfo['providerName']
		providerDetailsHistoryInfo.providerPostalAddress = entryInfo['providerPostalAddress']
		providerDetailsHistoryInfo.purposeText = entryInfo['purposeText']
		providerDetailsHistoryInfo.requestedDataText = entryInfo['requestedDataText']
		providerDetailsHistoryInfo.termsOfUsageText = entryInfo['termsOfUsageText']
	}
}
