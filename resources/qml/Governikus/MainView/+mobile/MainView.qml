/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: sectionPage

	Accessible.name: qsTr("Main view")
	Accessible.description: qsTr("This is the main view of the AusweisApp2.")

	title: qsTr("Start")

	GridLayout {
		anchors {
			fill: parent
			margins: Constants.pane_padding
		}

		flow: GridLayout.TopToBottom
		columnSpacing: Constants.component_spacing
		rowSpacing: Constants.component_spacing
		rows: Constants.is_tablet ? 2 : 4

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Check device and ID card")
			image: "qrc:///images/mobile/device.svg"

			onClicked: navBar.show(UiModule.CHECK_ID_CARD)
		}

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Change my (Transport) PIN")
			image: "qrc:///images/material_lock.svg"

			onClicked: navBar.show(UiModule.PINMANAGEMENT)
		}

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("See my personal data")
			image: "qrc:///images/mydata.svg"

			onClicked: navBar.show(UiModule.IDENTIFY)
		}

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignHCenter

			//: LABEL ANDROID IOS
			title: qsTr("Provider")
			image: "qrc:///images/provider.svg"

			onClicked: navBar.show(UiModule.PROVIDER)
		}
	}
}
