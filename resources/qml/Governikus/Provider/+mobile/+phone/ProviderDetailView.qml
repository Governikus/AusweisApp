/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: baseItem

	readonly property real tabBarSpacing: Constants.is_layout_ios ? Constants.component_spacing : 0
	property alias providerModelItem: provider.modelItem

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	title: provider.shortName
	titleBarColor: Category.displayColor(provider.category)

	ProviderModelItem {
		id: provider
	}

	header: ProviderHeader {
		id: ownHeader
		width: baseItem.width
		selectedProvider: provider
	}

	content: Item {
		height: swipeBar.height + swipeViewBackground.height + 2 * Constants.component_spacing + tabBarSpacing
		width: baseItem.width

		TabBar {
			id: swipeBar
			spacing: 0
			width: Constants.is_layout_android ? parent.width : parent.width * 0.666 // Each tab of the 2 tabs gets 1/3 of the parent width, the rest ist padding
			height: descriptionButton.implicitHeight
			anchors.top: parent.top
			anchors.topMargin: Constants.component_spacing
			anchors.horizontalCenter: parent.horizontalCenter

			currentIndex: swipeView.currentIndex

			ProviderDetailTab {
				id: descriptionButton
				//: LABEL ANDROID_PHONE IOS_PHONE
				text: qsTr("DESCRIPTION") + SettingsModel.translationTrigger
			}

			ProviderDetailTab {
				id: contactButton
				//: LABEL ANDROID_PHONE IOS_PHONE
				text: qsTr("CONTACT") + SettingsModel.translationTrigger
			}
		}

		Rectangle {
			id: swipeViewBackground

			height: swipeView.height
			width: parent.width
			anchors.top: swipeBar.bottom
			anchors.topMargin: tabBarSpacing
			anchors.horizontalCenter: swipeBar.horizontalCenter

			color: Style.color.background_pane

			SwipeView {
				id: swipeView

				height: Math.max(providerText.implicitHeight, providerInfo.implicitHeight)
				anchors.left: parent.left
				anchors.top: parent.top
				anchors.right: parent.right

				currentIndex: swipeBar.currentIndex
				clip: true

				GText {
					id: providerText

					Accessible.onScrollDownAction: baseItem.scrollPageDown()
					Accessible.onScrollUpAction: baseItem.scrollPageUp()

					padding: Constants.component_spacing
					//: LABEL ANDROID_PHONE IOS_PHONE
					text: (!!provider.longDescription ? provider.longDescription : qsTr("Description not available")) + SettingsModel.translationTrigger
					horizontalAlignment: Text.AlignLeft
				}

				ProviderContactTab {
					id: providerInfo

					contactModel: provider.contactModel
				}
			}
		}
	}
}
