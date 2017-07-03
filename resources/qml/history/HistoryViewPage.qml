import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QtControls

import "../"
import "../global"

SectionPage {
	id: baseItem

	property var modelItem

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: modelItem ? modelItem.subject : ""; font.bold: true }
	titleBarColor: modelItem ? Category.displayColor(modelItem.provider_category) : ""

	readonly property real headerHeight: Utils.dp(200)

	header: ProviderHeader {
		id: providerHeader
		width: baseItem.width
		height: headerHeight

		address: modelItem ? modelItem.provider_address : ""
		providerIcon: !modelItem ? "" : modelItem.provider_icon !== "" ? modelItem.provider_icon : Category.buttonImageSource(modelItem.provider_category)
		providerImage: !modelItem ? "" : modelItem.provider_image !== "" ? modelItem.provider_image : Category.backgroundImageSource(modelItem.provider_category)
		transparentColor: modelItem ? Category.displayColor(modelItem.provider_category) : ""
	}

	content: Item {
		width: baseItem.width
		height: baseItem.height

		CustomSwipeBar {
			id: swipeBar
			currentIndex: swipeView.currentIndex
			anchors.top: parent.top
			anchors.topMargin: Utils.dp(20)
		}

		Rectangle {
			anchors.fill: swipeView
			color: Constants.background_color
		}

		QtControls.SwipeView {
			id: swipeView
			anchors.top: swipeBar.bottom
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			width: baseItem.width
			currentIndex: swipeBar.currentIndex
			clip: true

			Item {
				Rectangle {
					anchors.fill: parent

					ProviderContactTab {
						anchors.fill: parent

						homepage: modelItem ? modelItem.provider_homepage : ""
						phone: modelItem ? modelItem.provider_phone : ""
						email: modelItem ? modelItem.provider_email : ""
						postalAddress: modelItem ? modelItem.provider_postaladdress : ""
					}
				}
			}

			HistoryListView {
				id: detailsHistoryListView

				onVisibleChanged: {
					if (visible) {
						historyModel.filter.setFilterFixedString(modelItem.subject)
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
