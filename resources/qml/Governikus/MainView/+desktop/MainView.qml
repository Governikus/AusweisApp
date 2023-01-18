/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: sectionPage
	GridLayout {
		anchors.fill: parent
		anchors.margins: Constants.component_spacing * 2
		columnSpacing: Constants.component_spacing
		columns: 5
		rowSpacing: Constants.component_spacing * 2
		rows: 3

		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/mydata.svg"

			//: LABEL DESKTOP
			title: qsTr("See my<br>personal data")

			onClicked: sectionPage.nextView(UiModule.SELF_AUTHENTICATION)
		}
		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large
			orientation: Qt.Vertical
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/provider.svg"

			//: LABEL DESKTOP
			title: qsTr("Provider")

			onClicked: sectionPage.nextView(UiModule.PROVIDER)
		}
		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large
			orientation: Qt.Vertical
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/material_history.svg"

			//: LABEL DESKTOP
			title: qsTr("History")

			onClicked: sectionPage.nextView(UiModule.HISTORY)
		}
		GSeparator {
			Layout.columnSpan: 5
			Layout.fillWidth: true
			Layout.preferredHeight: Style.dimens.separator_size_large
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/material_settings.svg"

			//: LABEL DESKTOP
			title: qsTr("Settings")

			onClicked: sectionPage.nextView(UiModule.SETTINGS)
		}
		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large
			orientation: Qt.Vertical
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/material_lock.svg"

			//: LABEL DESKTOP
			title: qsTr("Change my<br>(Transport) PIN")

			onClicked: sectionPage.nextView(UiModule.PINMANAGEMENT)
		}
		GSeparator {
			Layout.fillHeight: true
			Layout.preferredWidth: Style.dimens.separator_size_large
			orientation: Qt.Vertical
		}
		Tile {
			Layout.fillHeight: true
			Layout.fillWidth: true
			activeFocusOnTab: true
			image: "qrc:/images/material_help.svg"

			//: LABEL DESKTOP
			title: qsTr("Help")

			onClicked: sectionPage.nextView(UiModule.HELP)
		}
	}
}
