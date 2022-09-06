/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: baseItem

	readonly property color titleBarColor: Style.currentTheme.highContrast ? Style.color.background_pane : Category.displayColor(provider.category)
	signal showDetailView(var pModel)
	property alias historyModelItem: provider.modelItem
	property alias providerModelItem: provider.modelItem

	titleBarAction: TitleBarAction {
		text: provider.shortName
		helpTopic: "providerDetails"
	}

	ProviderModelItem {
		id: provider
	}

	Item {
		id: mainContent

		anchors.fill: parent

		Rectangle {
			id: imageHeader

			height: Math.floor(baseItem.height * 0.5)
			width: parent.width
			anchors.top: parent.top

			color: baseItem.titleBarColor

			RowLayout {
				anchors.fill: parent

				spacing: 0

				Image {
					Layout.fillHeight: true
					Layout.preferredWidth: Math.floor(parent.width * 0.6)

					source: provider.image
					sourceSize.height: height
					asynchronous: true
					fillMode: Image.PreserveAspectCrop
				}

				ProviderContactInfo {
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.margins: Constants.pane_spacing
					Layout.leftMargin: Constants.pane_spacing * 2
					Layout.rightMargin: Constants.pane_spacing * 2

					activeFocusOnTab: true

					contactModel: provider.contactModel
				}
			}
		}

		ProviderDetailButtonBar {
			id: buttonBar

			anchors.top: imageHeader.bottom

			selectedCategory: provider.category
			providerIcon: provider.icon
			address: provider.address
			shortName: provider.shortName
			longName: provider.longName
			buttonColor: baseItem.titleBarColor
		}

		RowLayout {
			id: lowerRow

			anchors.top: buttonBar.bottom
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Constants.component_spacing

			spacing: Constants.component_spacing

			Item {

				Layout.fillHeight: true
				Layout.fillWidth: true

				ScrollablePane {
					id: leftPane

					anchors.fill: parent

					activeFocusOnTab: true

					//: LABEL DESKTOP
					title: qsTr("Description")

					onVisibleChanged: scrollYPositionIntoView(0)

					GText {
						id: leftColumn

						width: parent.width

						activeFocusOnTab: true

						//: LABEL DESKTOP
						text: !!provider.longDescription ? provider.longDescription : qsTr("The provider did not provide a description.")
						textStyle: Style.text.normal
						textFormat: Text.RichText

						FocusFrame {}
					}
				}
			}

			Item {

				Layout.fillHeight: true
				Layout.fillWidth: true

				ScrollablePane {
					id: rightPane

					anchors.fill: parent

					activeFocusOnTab: true

					//: LABEL DESKTOP
					title: qsTr("History")

					onVisibleChanged: scrollYPositionIntoView(0)

					ProviderDetailHistory {
						id: rightColumn

						width: parent.width
					}
				}
			}
		}
	}
}
