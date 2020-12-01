/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("FAQ")
		//: LABEL DESKTOP_QML
		description: qsTr("Do you have further questions about %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_live_help.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language)
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: Qt.openUrlExternally(buttonTooltip)
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Support")
		//: LABEL DESKTOP_QML
		description: qsTr("Do you need further support?")
		iconSource: "qrc:/images/material_help.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language)
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: Qt.openUrlExternally(buttonTooltip)
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Online help")
		//: LABEL DESKTOP_QML
		description: qsTr("Do you need further help?")
		iconSource: "qrc:/images/desktop/material_menu_book.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website")
		buttonTooltip: ApplicationModel.onlineHelpUrl("index")
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: ApplicationModel.openOnlineHelp("index")
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Release notes")
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to view the release notes of %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_new_releases.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website")
		buttonTooltip: ApplicationModel.releaseNotesUrl
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: Qt.openUrlExternally(ApplicationModel.releaseNotesUrl)
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Setup assistant")
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to run the setup assistant again?")
		iconSource: "qrc:/images/desktop/material_assistant.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Start setup assistant")
		onClicked: sectionPage.nextView(SectionPage.Views.SetupAssistant)
	}
}
