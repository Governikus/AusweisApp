/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Provider 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem

	readonly property real tabBarSpacing: Constants.is_layout_ios ? Constants.component_spacing : 0
	property alias providerModelItem: provider.modelItem

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	title: provider.shortName
	titleBarColor: Category.displayColor(provider.category)
	titleBarOpacity: header.titleBarOpacity
	contentBehindTitlebar: true

	ProviderModelItem {
		id: provider
	}

	content: Column {
		ProviderHeader {
			id: header

			width: baseItem.width

			selectedProvider: provider
		}

		Item {
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
					text: qsTr("DESCRIPTION")
				}

				ProviderDetailTab {
					id: contactButton
					//: LABEL ANDROID_PHONE IOS_PHONE
					text: qsTr("CONTACT")
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
						text: (!!provider.longDescription ? provider.longDescription : qsTr("The provider did not provide a description."))
						textFormat: Text.RichText
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
}
