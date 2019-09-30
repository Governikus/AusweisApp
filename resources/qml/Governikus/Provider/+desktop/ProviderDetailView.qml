/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.0
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

SectionPage {
	id: baseItem

	readonly property color titleBarColor: Category.displayColor(provider.category)
	readonly property real titleBarOpacity: 1
	signal showDetailView(var pModel)
	property alias historyModelItem: provider.modelItem
	property alias providerModelItem: provider.modelItem

	Accessible.name: qsTr("Provider detail view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This view shows a detailed description of a service provider.") + SettingsModel.translationTrigger

	titleBarAction: TitleBarAction {
		text: provider.shortName
		helpTopic: "providerPage"
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

				FocusFrame {
					framee: providerContactInfo
					scope: providerContactInfo
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

					//: LABEL DESKTOP_QML
					title: qsTr("Description") + SettingsModel.translationTrigger

					GText {
						id: leftColumn

						width: parent.width

						Accessible.name: qsTr("Description of the service provider.") + SettingsModel.translationTrigger
						Accessible.description: text
						Accessible.role: Accessible.StaticText
						activeFocusOnTab: true

						//: LABEL DESKTOP_QML
						text: !!provider.longDescription ? provider.longDescription : qsTr("The service provider did not provide a description.") + SettingsModel.translationTrigger
						textStyle: Style.text.normal_inverse
					}
				}

				FocusFrame {
					framee: leftPane
					scope: leftColumn
				}
			}

			Item {

				Layout.fillHeight: true
				Layout.fillWidth: true

				ScrollablePane {
					id: rightPane

					anchors.fill: parent

					//: LABEL DESKTOP_QML
					title: qsTr("History") + SettingsModel.translationTrigger

					ProviderDetailHistory {
						id: rightColumn

						width: parent.width

						activeFocusOnTab: true
					}
				}

				FocusFrame {
					framee: rightPane
					scope: rightColumn
				}
			}
		}
	}

	FocusPoint {
		color: Constants.black
	}
}
