/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
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

	property alias historyModelItem: provider.modelItem
	property alias providerModelItem: provider.modelItem
	readonly property color titleBarColor: Style.currentTheme.highContrast ? Style.color.background_pane : Category.displayColor(provider.category)

	signal showDetailView(var pModel)

	titleBarAction: TitleBarAction {
		helpTopic: "providerDetails"
		text: provider.shortName
	}

	ProviderModelItem {
		id: provider
	}
	Item {
		id: mainContent
		anchors.fill: parent

		Rectangle {
			id: imageHeader
			anchors.top: parent.top
			color: baseItem.titleBarColor
			height: Math.floor(baseItem.height * 0.5)
			width: parent.width

			RowLayout {
				anchors.fill: parent
				spacing: 0

				Image {
					Layout.fillHeight: true
					Layout.preferredWidth: Math.floor(parent.width * 0.6)
					asynchronous: true
					fillMode: Image.PreserveAspectCrop
					source: provider.image
					sourceSize.height: height
				}
				ProviderContactInfo {
					Layout.fillHeight: true
					Layout.fillWidth: true
					Layout.leftMargin: Constants.pane_spacing * 2
					Layout.margins: Constants.pane_spacing
					Layout.rightMargin: Constants.pane_spacing * 2
					activeFocusOnTab: true
					contactModel: provider.contactModel
				}
			}
		}
		ProviderDetailButtonBar {
			id: buttonBar
			address: provider.address
			anchors.top: imageHeader.bottom
			buttonColor: baseItem.titleBarColor
			longName: provider.longName
			providerIcon: provider.icon
			selectedCategory: provider.category
			shortName: provider.shortName
		}
		RowLayout {
			id: lowerRow
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.margins: Constants.component_spacing
			anchors.right: parent.right
			anchors.top: buttonBar.bottom
			spacing: Constants.component_spacing

			Item {
				Layout.fillHeight: true
				Layout.fillWidth: true

				ScrollablePane {
					id: leftPane
					activeFocusOnTab: true
					anchors.fill: parent

					//: LABEL DESKTOP
					title: qsTr("Description")

					onVisibleChanged: scrollYPositionIntoView(0)

					GText {
						id: leftColumn
						activeFocusOnTab: true

						//: LABEL DESKTOP
						text: !!provider.longDescription ? provider.longDescription : qsTr("The provider did not provide a description.")
						textFormat: Text.RichText
						textStyle: Style.text.normal
						width: parent.width

						FocusFrame {
						}
					}
				}
			}
			Item {
				Layout.fillHeight: true
				Layout.fillWidth: true

				ScrollablePane {
					id: rightPane
					activeFocusOnTab: true
					anchors.fill: parent

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
