/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP
		title: qsTr("Online help")
		//: LABEL DESKTOP
		description: qsTr("Do you have questions about %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_menu_book.svg"
		//: LABEL DESKTOP
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

		//: LABEL DESKTOP
		title: qsTr("Video tutorials")
		//: LABEL DESKTOP
		description: qsTr("Do you want to see the video tutorials?")
		iconSource: "qrc:///images/desktop/material_video.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/videotutorials".arg(SettingsModel.language)
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: Qt.openUrlExternally(buttonTooltip)
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP
		title: qsTr("FAQ")
		//: LABEL DESKTOP
		description: qsTr("Do you have further questions about %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_live_help.svg"
		//: LABEL DESKTOP
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

		//: LABEL DESKTOP
		title: qsTr("Support")
		//: LABEL DESKTOP
		description: qsTr("Do you need further support?")
		iconSource: "qrc:/images/material_help.svg"
		//: LABEL DESKTOP
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

		//: LABEL DESKTOP
		title: qsTr("Accessibility statement")
		//: LABEL DESKTOP
		description: qsTr("Do you want to read the accessibility statement?")
		iconSource: "qrc:/images/desktop/material_a11y.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language)
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		onClicked: Qt.openUrlExternally(buttonTooltip)
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP
		title: qsTr("Setup assistant")
		//: LABEL DESKTOP
		description: qsTr("Do you want to run the setup assistant again?")
		iconSource: "qrc:/images/desktop/material_assistant.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Start setup assistant")
		onClicked: sectionPage.nextView(UiModule.TUTORIAL)
	}
}
