import QtQuick 2.6
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0

SectionPage {
	id: baseItem
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: provider.shortName }
	titleBarColor: Category.displayColor(provider.category)

	property alias providerModelItem: provider.modelItem
	ProviderModelItem {
		id: provider
	}


	header: ProviderHeader {
		id: ownHeader
		width: baseItem.width
		selectedProvider: provider
	}

	content: Item {
		height: swipeBar.height + swipeViewBackground.height + Constants.component_spacing
		width: baseItem.width

		Rectangle {
			id: swipeBar
			anchors.top: header.bottom
			anchors.topMargin: Utils.dp(20)
			anchors.horizontalCenter: parent.horizontalCenter

			width: descriptionTab.width + contactTab.width + 2 * border.width
			height: descriptionTab.height  + 2 * border.width

			border.color: Constants.blue
			border.width: Utils.dp(1)
			radius: Utils.dp(3)
			clip: true

			Row {
				id: row
				readonly property int maxContentWidth: Math.max(descriptionText.contentWidth, contactText.contentWidth)

				anchors.centerIn: parent
				Rectangle {
					id: descriptionTab
					width: row.maxContentWidth + Utils.dp(6)
					height: descriptionText.contentHeight + Utils.dp(6)
					color: swipeView.currentIndex === 0 ? Constants.blue : descriptiontMouseArea.pressed ? PlatformConstants.blue_light : "transparent"
					Text {
						id: descriptionText
						anchors.centerIn: parent
						color: swipeView.currentIndex === 0 ? "white" : Constants.blue
						text: qsTr("Description") + settingsModel.translationTrigger
					}
					MouseArea {
						id: descriptiontMouseArea
						anchors.fill: parent
						onClicked: swipeView.currentIndex = 0
					}
				}
				Rectangle {
					id: contactTab
					width: row.maxContentWidth + Utils.dp(6)
					height: contactText.contentHeight + Utils.dp(6)
					color: swipeView.currentIndex === 1 ? Constants.blue : contactMouseArea.pressed ? PlatformConstants.blue_light : "transparent"
					Text {
						id: contactText
						anchors.centerIn: parent
						color: swipeView.currentIndex === 1 ? "white" : Constants.blue
						text: qsTr("Contact") + settingsModel.translationTrigger
					}
					MouseArea {
						id: contactMouseArea
						anchors.fill: parent
						onClicked: swipeView.currentIndex = 1
					}
				}
			}
		}

		Rectangle {
			id: swipeViewBackground
			anchors.top: swipeBar.bottom
			anchors.horizontalCenter: swipeBar.horizontalCenter
			height: swipeView.height + 2 * Constants.component_spacing
			width: parent.width

			SwipeView {
				id: swipeView
				height: Math.max(providerText.contentHeight, providerInfo.contentHeight)
				anchors.margins: Constants.component_spacing
				anchors.left: parent.left
				anchors.top: parent.top
				anchors.right: parent.right
				clip: true

				Text {
					id: providerText
					text: (!!provider.longDescription ? provider.longDescription : qsTr("Description not available")) + settingsModel.translationTrigger
					horizontalAlignment: Text.AlignLeft
					wrapMode: Text.WordWrap
					font.pixelSize: Constants.normal_font_size
				}

				ProviderContactTab {
					id: providerInfo
					contactModel: provider.contactModel
				}
			}
		}
	}
}
