/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SelfAuthModel
import Governikus.Type.UiModule

SectionPage {
	id: sectionPage

	GridLayout {
		columnSpacing: Constants.component_spacing
		columns: 2
		rowSpacing: Constants.component_spacing
		rows: 2

		anchors {
			bottomMargin: anchors.topMargin
			fill: parent
			leftMargin: anchors.rightMargin
			rightMargin: Constants.component_spacing + Math.max(0, (sectionPage.width - sectionPage.height) / 2)
			topMargin: Constants.component_spacing + Math.max(0, (sectionPage.height - sectionPage.width) / 2)
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/lock.svg"

			//: LABEL DESKTOP
			title: qsTr("Change PIN")

			onClicked: sectionPage.nextView(UiModule.PINMANAGEMENT)
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/mydata.svg"

			//: LABEL DESKTOP
			title: qsTr("See my<br>personal data")

			onClicked: sectionPage.nextView(UiModule.SELF_AUTHENTICATION)
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/desktop/settings.svg"

			//: LABEL DESKTOP
			title: qsTr("Settings")

			onClicked: sectionPage.nextView(UiModule.SETTINGS)
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/desktop/help.svg"

			//: LABEL DESKTOP
			title: qsTr("Help")

			onClicked: sectionPage.nextView(UiModule.HELP)
		}
	}
}
