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

		TabBar {
			id: swipeBar
			height: firstButton.implicitHeight
			anchors.top: parent.top
			anchors.topMargin: Constants.component_spacing
			anchors.left: parent.left
			anchors.right: parent.right

			currentIndex: swipeView.currentIndex

			TabButton {
				id: firstButton
				padding: Utils.dp(10)
				// TODO: Workaround, use contentItem when switching to Qt 5.7.1
				//       See https://bugreports.qt.io/browse/QTBUG-50992
				text: qsTr("DESCRIPTION") + settingsModel.translationTrigger

/*
				contentItem: Text {
					text: qsTr("DESCRIPTION") + settingsModel.translationTrigger
					font.pixelSize: Constants.normal_font_size
					elide: Text.ElideRight
					opacity: enabled ? 1 : 0.3
					color: !parent.checked ? "black" : parent.pressed ? "black" : Constants.blue
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}
*/
			}

			TabButton {
				padding: Utils.dp(10)
				// TODO: Workaround, use contentItem when switching to Qt 5.7.1
				//       See https://bugreports.qt.io/browse/QTBUG-50992
				text: qsTr("CONTACT") + settingsModel.translationTrigger

/*
				contentItem: Text {
					text: qsTr("CONTACT") + settingsModel.translationTrigger
					font.pixelSize: Constants.normal_font_size
					elide: Text.ElideRight
					opacity: enabled ? 1 : 0.3
					color: !parent.checked ? "black" : parent.pressed ? "black" : Constants.blue
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}
*/
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
