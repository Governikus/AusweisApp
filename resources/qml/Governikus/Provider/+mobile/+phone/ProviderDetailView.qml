import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

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

		TabBar {
			id: swipeBar
			spacing: 0
			width: Constants.is_layout_android ? parent.width : Utils.dp(220)
			height: descriptionButton.implicitHeight
			anchors.top: parent.top
			anchors.topMargin: Constants.component_spacing
			anchors.horizontalCenter: parent.horizontalCenter

			currentIndex: swipeView.currentIndex

			ProviderDetailTab {
				id: descriptionButton
				text: qsTr("DESCRIPTION") + settingsModel.translationTrigger
			}

			ProviderDetailTab {
				id: contactButton
				text: qsTr("CONTACT") + settingsModel.translationTrigger
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

				currentIndex: swipeBar.currentIndex
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
