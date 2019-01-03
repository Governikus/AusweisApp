import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0

SectionPage {
	id: sectionPage

	KeyNavigation.tab: tileIdentify
	Accessible.role: Accessible.Grouping
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

				title: qsTr("Identify")
				image: "qrc:/images/desktop/main_identify.svg"

				onClicked: selfAuthModel.startWorkflow()

				KeyNavigation.tab: tileProvider
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Constants.grey_light
			}

			Tile {
				id: tileProvider

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				title: qsTr("Provider")
				image: "qrc:/images/desktop/main_provider.svg"

				onClicked: sectionPage.nextView(SectionPage.Views.Provider)

				KeyNavigation.tab: tileHistory
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Constants.grey_light
			}

			Tile {
				id: tileHistory

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				title: qsTr("History")
				image: "qrc:/images/desktop/main_history.svg"

				KeyNavigation.tab: tileSettings
			}
		}

		Rectangle {
			height: view.separatorLineWidth
			width: parent.width - view.horizontalItemSpace / 3
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.verticalCenter: parent.verticalCenter
			color: Constants.grey_light
		}

		Row {
			height: parent.verticalItemSpace
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom

			Tile {
				id: tileSettings

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				title: qsTr("Settings")
				image: "qrc:/images/desktop/settings_icon.svg"

				KeyNavigation.tab: tilePin
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Constants.grey_light
			}

			Tile {
				id: tilePin

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				title: qsTr("PIN management")
				image: "qrc:/images/desktop/main_pin.svg"

				KeyNavigation.tab: tileHelp
			}

			Rectangle {
				height: view.verticalItemSpace * 2/3
				width: view.separatorLineWidth
				anchors.verticalCenter: parent.verticalCenter
				color: Constants.grey_light
			}

			Tile {
				id: tileHelp

				height: view.verticalItemSpace
				width: view.horizontalItemSpace

				title: qsTr("Help")
				image: "qrc:/images/desktop/help_icon.svg"

				KeyNavigation.tab: sectionPage.navSuccessor
			}
		}
	}
}
