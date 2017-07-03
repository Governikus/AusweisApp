import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import "../"
import "../global"

SectionPage {
	id: baseItem
	property var historyModelItem
	readonly property real headerHeight: Utils.dp(200)
	ProviderModelItem {
		id: provider
		modelItem: baseItem.historyModelItem
	}

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: historyModelItem ? historyModelItem.subject : ""; font.bold: true }
	titleBarColor: !!provider.category ? Category.displayColor(provider.category) : ""


	header: ProviderHeader {
		id: providerHeader
		width: baseItem.width
		selectedProvider: provider
	}

	content: Item {
		height: swipeBar.height + swipeViewBackground.height + Constants.component_spacing
		width: baseItem.width

		CustomSwipeBar {
			id: swipeBar
			currentIndex: swipeView.currentIndex
			anchors.top: parent.top
			anchors.topMargin: Utils.dp(20)
		}

		Rectangle {
			id: swipeViewBackground
			anchors.top: swipeBar.bottom
			anchors.horizontalCenter: swipeBar.horizontalCenter
			height: swipeView.height + 2 * Constants.component_spacing
			width: parent.width

			SwipeView {
				id: swipeView
				height: Math.max(providerInfo.contentHeight, detailsHistoryListView.contentHeight)
				anchors.margins: Constants.component_spacing
				anchors.left: parent.left
				anchors.top: parent.top
				anchors.right: parent.right

				currentIndex: swipeBar.currentIndex
				clip: true

				ProviderContactTab {
					id: providerInfo
					contactModel: provider.contactModel
				}

				HistoryListView {
					id: detailsHistoryListView

					onVisibleChanged: {
						if (visible) {
							historyModel.filter.setFilterFixedString(historyModelItem.subject)
						}
					}

					listViewModel: historyModel.filter
					delegate: HistoryListViewDelegate {
						id: detailsHistoryDelegate
						showDetail: true
						listModel: historyModel.filter
					}
				}
			}
		}
	}
}
