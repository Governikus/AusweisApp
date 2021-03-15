/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: sectionPage

	Accessible.name: qsTr("Main view")
	Accessible.description: qsTr("This is the main view of the AusweisApp2.")

	GridLayout {
		anchors.fill: parent
		anchors.margins: Constants.component_spacing * 2

		columns: 5
		rows: 3
		columnSpacing: Constants.component_spacing
		rowSpacing: Constants.component_spacing * 2

		Tile {
			Layout.fillWidth: true
			Layout.fillHeight: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			title: qsTr("See my<br>personal data")
			image: "qrc:/images/mydata.svg"

			onClicked: sectionPage.nextView(UiModule.SELF_AUTHENTICATION)
		}

		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large

			orientation: Qt.Vertical
		}

		Tile {
			Layout.fillWidth: true
			Layout.fillHeight: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			title: qsTr("Provider")
			image: "qrc:/images/provider.svg"

			onClicked: sectionPage.nextView(UiModule.PROVIDER)
		}

		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large

			orientation: Qt.Vertical
		}

		Tile {
			Layout.fillWidth: true
			Layout.fillHeight: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			title: qsTr("History")
			image: "qrc:/images/history.svg"

			onClicked: sectionPage.nextView(UiModule.HISTORY)
		}

		GSeparator {
			Layout.fillWidth: true
			Layout.preferredHeight: Style.dimens.separator_size_large
			Layout.columnSpan: 5
		}

		Tile {
			Layout.fillWidth: true
			Layout.fillHeight: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			title: qsTr("Settings")
			image: "qrc:/images/material_settings.svg"

			onClicked: sectionPage.nextView(UiModule.SETTINGS)
		}

		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large

			orientation: Qt.Vertical
		}

		Tile {
			Layout.fillWidth: true
			Layout.fillHeight: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			title: qsTr("Change my<br>(Transport) PIN")
			image: "qrc:/images/material_lock.svg"

			onClicked: sectionPage.nextView(UiModule.PINMANAGEMENT)
		}

		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large

			orientation: Qt.Vertical
		}

		Tile {
			Layout.fillWidth: true
			Layout.fillHeight: true

			activeFocusOnTab: true

			//: LABEL DESKTOP_QML
			title: qsTr("Help")
			image: "qrc:/images/material_help.svg"

			onClicked: sectionPage.nextView(UiModule.HELP)
		}
	}
}
