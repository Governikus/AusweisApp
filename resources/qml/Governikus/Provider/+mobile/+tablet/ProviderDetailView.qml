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

	property alias historyModelItem: provider.modelItem
	property var openHistoryInfoFunc: function (entryInfo) {
		providerDetailsHistoryInfo.visible = true;
		providerDetailsHistoryInfo.providerName = entryInfo['providerName'];
		providerDetailsHistoryInfo.providerPostalAddress = entryInfo['providerPostalAddress'];
		providerDetailsHistoryInfo.purposeText = entryInfo['purposeText'];
		providerDetailsHistoryInfo.requestedDataText = entryInfo['requestedDataText'];
		providerDetailsHistoryInfo.termsOfUsageText = entryInfo['termsOfUsageText'];
	}
	property alias providerModelItem: provider.modelItem
	readonly property real titleBarOpacity: 1

	title: historyModelItem && historyModelItem.subject ? historyModelItem.subject : provider.shortName
	titleBarColor: Category.displayColor(provider.category)

	content: Column {
		height: childrenRect.height + Constants.component_spacing
		width: baseItem.width

		Rectangle {
			color: baseItem.titleBarColor
			height: baseItem.height / 2
			width: parent.width

			RowLayout {
				anchors.fill: parent
				spacing: 0

				Item {
					Layout.fillHeight: true
					Layout.preferredWidth: baseItem.width * 2 / 3

					Image {
						anchors.left: parent.left
						anchors.verticalCenter: parent.verticalCenter
						asynchronous: true
						fillMode: Image.PreserveAspectFit
						height: parent.height
						source: provider.image

						Rectangle {
							width: parent.width / 4

							gradient: Gradient {
								orientation: Gradient.Horizontal

								GradientStop {
									color: Style.color.transparent
									position: 0.0
								}
								GradientStop {
									color: Category.displayColor(provider.category)
									position: 0.95
								}
							}

							anchors {
								bottom: parent.bottom
								right: parent.right
								top: parent.top
							}
						}
					}
				}
				ProviderContactInfo {
					Layout.fillHeight: true
					Layout.fillWidth: true
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
					address: provider.address
					providerIcon: provider.icon
					selectedCategory: provider.category
					titleBarColor: baseItem.titleBarColor
				}
				GPane {
					id: leftPane
					height: lowerRow.height - (buttonBar.height + Constants.pane_padding)

					anchors {
						left: parent.left
						margins: Constants.component_spacing
						right: parent.right
						top: buttonBar.bottom
					}
				}
				ProviderDetailDescription {
					id: leftColumn
					anchors.left: parent.left
					anchors.margins: 2 * Constants.pane_padding
					anchors.right: parent.right
					anchors.top: buttonBar.bottom
					description: provider.longDescription

					onScrollDescriptionDown: baseItem.scrollPageDown()
					onScrollDescriptionUp: baseItem.scrollPageUp()
				}
			}
			Item {
				height: 1
				width: lowerRow.width / 3 - Constants.component_spacing

				GPane {
					id: rightPane
					height: lowerRow.height - Constants.pane_padding

					anchors {
						left: parent.left
						right: parent.right
						top: parent.top
						topMargin: Constants.component_spacing
					}
				}
				ProviderDetailHistory {
					id: rightColumn
					anchors.left: parent.left
					anchors.leftMargin: Constants.pane_padding
					anchors.right: parent.right
					anchors.rightMargin: Constants.pane_padding
					anchors.top: parent.top
					anchors.topMargin: 2 * Constants.pane_padding
					openHistoryInfoFunc: baseItem.openHistoryInfoFunc

					onScrollHistoryDown: baseItem.scrollPageDown()
					onScrollHistoryUp: baseItem.scrollPageUp()
				}
			}
		}
	}
	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: {
			if (providerDetailsHistoryInfo.visible) {
				providerDetailsHistoryInfo.visible = false;
			} else {
				pop();
			}
		}
	}
	rightTitleBarAction: Item {
	}

	ProviderModelItem {
		id: provider
	}
	ProviderDetailHistoryInfo {
		id: providerDetailsHistoryInfo
		anchors.left: baseItem.left
		anchors.top: baseItem.top
		height: parent.height
		visible: false
		width: parent.width
	}
}
