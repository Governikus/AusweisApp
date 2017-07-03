import QtQuick 2.6

import "."
import ".."
import "../global"


SectionPage {
	id: baseItem
	readonly property TitleBarMenuAction leftTitleBarAction: TitleBarMenuAction {
		state: "back"
		onClicked: {
			if (providerDetailsHistoryInfo.visible) {
				providerDetailsHistoryInfo.visible = false
			}
			else {
				pop()
			}
		}
	}
	readonly property TitleBarAction headerTitleBarAction: TitleBarAction { text: provider.shortName; font.bold: true }
	readonly property Item rightTitleBarAction: Item {}
	readonly property color titleBarColor: Category.displayColor(provider.category)
	readonly property real titleBarOpacity: 1

	property alias historyModelItem: provider.modelItem
	property alias providerModelItem: provider.modelItem
	ProviderModelItem {
		id: provider
	}


	content: Column {
		id: mainContent
		height: childrenRect.height + Constants.component_spacing
		width: baseItem.width

		Row {
			height: baseItem.height / 2
			width: parent.width

			Item {
				height: parent.height
				width: baseItem.width * 2 / 3
				anchors.top: parent.top

				Image {
					id: image
					source: !!provider.image ? provider.image : Category.backgroundImageSource(provider.category)
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

		Row {
			id: lowerRow
			height: childrenRect.height
			width: parent.width

			property int maxContentHeight: Math.max(leftPane.contentHeight + buttonBar.height, rightPane.contentHeight)

			Column {
				id: leftColumn
				width: lowerRow.width * 2 / 3
				spacing: Constants.pane_spacing

				ProviderDetailButtonBar {
					id: buttonBar
					selectedCategory: provider.category
					providerIcon: provider.icon
					address: provider.address
					titleBarColor: baseItem.titleBarColor
				}

				Pane {
					id: leftPane
					anchors.leftMargin: Constants.component_spacing
					anchors.rightMargin: Constants.component_spacing
					height: childrenRect.height - buttonBar.height + verticalSpace

					readonly property int contentHeight: childrenRect.height
					property int verticalSpace: lowerRow.maxContentHeight - contentHeight

					ProviderDetailDescription {
						id: descriptionData
						width: parent.width
						description: provider.longDescription
					}
				}
			}

			Column {
				width: lowerRow.width / 3 - Constants.component_spacing

				Item {
					height: Constants.component_spacing
					width: Constants.component_spacing
				}

				Pane {
					id: rightPane
					height: childrenRect.height + verticalSpace

					readonly property int contentHeight: childrenRect.height
					property int verticalSpace: lowerRow.maxContentHeight - contentHeight

					ProviderDetailHistory {
						id: historyData
						width: parent.width
						openHistoryInfoFunc: baseItem.openHistoryInfoFunc
					}
				}
			}
		}
	}

	property var openHistoryInfoFunc: function(entryInfo) {
		providerDetailsHistoryInfo.visible = true

		providerDetailsHistoryInfo.providerName = entryInfo['providerName']
		providerDetailsHistoryInfo.providerPostalAddress = entryInfo['providerPostalAddress']
		providerDetailsHistoryInfo.purposeText = entryInfo['purposeText']
		providerDetailsHistoryInfo.requestedDataText = entryInfo['requestedDataText']
		providerDetailsHistoryInfo.termsOfUsageText = entryInfo['termsOfUsageText']
	}

	ProviderDetailHistoryInfo {
		id: providerDetailsHistoryInfo

		height: parent.height
		width: parent.width

		anchors.top: baseItem.top
		anchors.left: baseItem.left

		visible: false
	}
}
