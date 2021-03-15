/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

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

	Accessible.name: qsTr("Provider detail view")
	Accessible.description: qsTr("This view shows a detailed description of a provider.")

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

		Row {
			id: imageHeader

			height: Math.floor(baseItem.height * 0.5)
			width: parent.width

			anchors.top: parent.top

			Image {
				id: image

				height: parent.height
				width: Math.floor(baseItem.width * 0.6)
				anchors.top: parent.top

				source: provider.image
				sourceSize.height: height
				asynchronous: true
				fillMode: Image.PreserveAspectCrop
			}

			Rectangle {
				height: parent.height
				width: Math.ceil(baseItem.width * 0.4)

				color: baseItem.titleBarColor

				ProviderContactInfo {
					id: providerContactInfo

					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.margins: Constants.pane_spacing
					anchors.leftMargin: Constants.pane_spacing * 2
					anchors.rightMargin: Constants.pane_spacing * 2

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
			shortDescription: provider.shortDescription
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

					//: LABEL DESKTOP_QML
					title: qsTr("Description")

					onVisibleChanged: scrollYPositionIntoView(0)

					GText {
						id: leftColumn

						width: parent.width

						activeFocusOnTab: true

						//: LABEL DESKTOP_QML
						text: !!provider.longDescription ? provider.longDescription : qsTr("The provider did not provide a description.")
						textStyle: Style.text.normal

						FocusFrame {
							borderColor: Style.color.focus_indicator
						}
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

					//: LABEL DESKTOP_QML
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

	FocusPoint {
		color: Style.color.focus_indicator
	}
}
