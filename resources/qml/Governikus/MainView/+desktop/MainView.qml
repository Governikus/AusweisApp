/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.ChangePinModel 1.0

SectionPage {
	id: sectionPage

	Accessible.name: qsTr("Main view")
	Accessible.description: qsTr("This is the main view of the AusweisApp2.")

	Item {
		id: view
		anchors.fill: parent

		readonly property int separatorLineWidth: Math.max(1, ApplicationModel.scaleFactor * 4)
		readonly property int horizontalItemSpace: (width -  2 * separatorLineWidth) / 3
		readonly property int verticalItemSpace: (height - separatorLineWidth) / 2

		Row {
			height: parent.verticalItemSpace
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top

			Tile {
				id: tileIdentify

				height: parent.height
				width: view.horizontalItemSpace

				//: LABEL DESKTOP_QML
				title: qsTr("Self-authentication") + SettingsModel.translationTrigger
				image: "qrc:/images/desktop/main_identify.svg"

				onClicked: sectionPage.nextView(SectionPage.Views.SelfAuthentication)

				activeFocusOnTab: true
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Style.color.border
			}

			Tile {
				id: tileProvider

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				//: LABEL DESKTOP_QML
				title: qsTr("Provider") + SettingsModel.translationTrigger
				image: "qrc:/images/desktop/main_provider.svg"

				onClicked: sectionPage.nextView(SectionPage.Views.Provider)

				activeFocusOnTab: true
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Style.color.border
			}

			Tile {
				id: tileHistory

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				//: LABEL DESKTOP_QML
				title: qsTr("History") + SettingsModel.translationTrigger
				image: "qrc:/images/desktop/main_history.svg"

				onClicked: sectionPage.nextView(SectionPage.Views.History)

				activeFocusOnTab: true
			}
		}

		Rectangle {
			height: view.separatorLineWidth
			width: parent.width - view.horizontalItemSpace / 3
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			color: Style.color.border
		}

		Row {
			height: parent.verticalItemSpace
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom

			Tile {
				id: tileSettings

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				//: LABEL DESKTOP_QML
				title: qsTr("Settings") + SettingsModel.translationTrigger
				image: "qrc:/images/desktop/settings_icon.svg"

				onClicked: sectionPage.nextView(SectionPage.Views.Settings)

				activeFocusOnTab: true
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Style.color.border
			}

			Tile {
				id: tilePin

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				//: LABEL DESKTOP_QML
				title: qsTr("PIN management") + SettingsModel.translationTrigger
				image: "qrc:/images/desktop/main_pin.svg"

				onClicked: ChangePinModel.startWorkflow()

				activeFocusOnTab: true
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Style.color.border
			}

			Tile {
				id: tileHelp

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				//: LABEL DESKTOP_QML
				title: qsTr("Help") + SettingsModel.translationTrigger
				image: "qrc:/images/desktop/help_icon.svg"

				activeFocusOnTab: true

				onClicked: sectionPage.nextView(SectionPage.Views.Information)
			}
		}
	}
}
