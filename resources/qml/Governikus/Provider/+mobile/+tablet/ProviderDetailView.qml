/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back
		onClicked: {
			if (providerDetailsHistoryInfo.visible) {
				providerDetailsHistoryInfo.visible = false
			}
			else {
				pop()
			}
		}
	}
	title: historyModelItem && historyModelItem.subject ? historyModelItem.subject : provider.shortName
	rightTitleBarAction: Item {}
	titleBarColor: Category.displayColor(provider.category)
	readonly property real titleBarOpacity: 1

	property alias historyModelItem: provider.modelItem
	property alias providerModelItem: provider.modelItem
	ProviderModelItem {
		id: provider
	}


	content: Column {
		height: childrenRect.height + Constants.component_spacing
		width: baseItem.width

		Rectangle {
			height: baseItem.height / 2
			width: parent.width

			color: baseItem.titleBarColor

			RowLayout {
				anchors.fill: parent

				spacing: 0

				Item {
					Layout.fillHeight: true
					Layout.preferredWidth: baseItem.width * 2 / 3

					Image {
						source: provider.image
						asynchronous: true
						height: parent.height
						fillMode: Image.PreserveAspectFit
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter

						Rectangle {
							width: parent.width / 4
							anchors {
								right: parent.right
								top: parent.top
								bottom: parent.bottom
							}

							gradient: Gradient {
								orientation: Gradient.Horizontal
								GradientStop { position: 0.0; color: Style.color.transparent }
								GradientStop { position: 0.95; color: Category.displayColor(provider.category) }
							}
						}
					}
				}

				ProviderContactInfo {
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.margins: Constants.groupbox_spacing

					contactModel: provider.contactModel
				}
			}
		}

		Row {
			id: lowerRow
			height: Math.max(buttonBar.height + leftColumn.height, rightColumn.height) + 3 * Constants.pane_padding
			width: parent.width

			Item {
				height: 1
				width: lowerRow.width * 2 / 3

				ProviderDetailButtonBar {
					id: buttonBar
					selectedCategory: provider.category
					providerIcon: provider.icon
					address: provider.address
					titleBarColor: baseItem.titleBarColor
				}

				GPane {
					id: leftPane

					anchors {
						margins: Constants.component_spacing
						top: buttonBar.bottom
						left: parent.left
						right: parent.right
					}
					height: lowerRow.height - (buttonBar.height + Constants.pane_padding)
				}

				ProviderDetailDescription {
					id: leftColumn

					anchors.margins: 2 * Constants.pane_padding
					anchors.top: buttonBar.bottom
					anchors.left: parent.left
					anchors.right: parent.right

					onScrollDescriptionDown: baseItem.scrollPageDown()
					onScrollDescriptionUp: baseItem.scrollPageUp()

					description: provider.longDescription
				}
			}

			Item {
				height: 1
				width: lowerRow.width / 3 - Constants.component_spacing

				GPane {
					id: rightPane
					anchors {
						topMargin: Constants.component_spacing
						top: parent.top
						left: parent.left
						right: parent.right
					}
					height: lowerRow.height - Constants.pane_padding
				}

				ProviderDetailHistory {
					id: rightColumn

					anchors.topMargin: 2 * Constants.pane_padding
					anchors.leftMargin: Constants.pane_padding
					anchors.rightMargin: Constants.pane_padding
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right

					onScrollHistoryDown: baseItem.scrollPageDown()
					onScrollHistoryUp: baseItem.scrollPageUp()

					openHistoryInfoFunc: baseItem.openHistoryInfoFunc
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
