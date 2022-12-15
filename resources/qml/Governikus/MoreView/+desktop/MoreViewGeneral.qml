/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0

ColumnLayout {
	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: ApplicationModel.onlineHelpUrl("index")
		//: LABEL DESKTOP
		description: qsTr("Do you have questions about %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_menu_book.svg"

		//: LABEL DESKTOP
		title: qsTr("Online help")

		onClicked: ApplicationModel.openOnlineHelp("index")
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/videotutorials".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Do you want to see the video tutorials?")
		iconSource: "qrc:///images/desktop/material_video.svg"

		//: LABEL DESKTOP
		title: qsTr("Video tutorials")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Do you have further questions about %1?").arg(Qt.application.name)
		iconSource: "qrc:/images/desktop/material_live_help.svg"

		//: LABEL DESKTOP
		title: qsTr("FAQ")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Do you need further support?")
		iconSource: "qrc:/images/material_help.svg"

		//: LABEL DESKTOP
		title: qsTr("Support")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Do you want to read the privacy statement?")
		iconSource: "qrc:/images/desktop/material_privacy.svg"

		//: LABEL DESKTOP
		title: qsTr("Privacy statement")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		buttonIconSource: "qrc:///images/material_open_in_new.svg"
		//: LABEL DESKTOP
		buttonText: qsTr("Open website")
		buttonTooltip: "https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language)
		//: LABEL DESKTOP
		description: qsTr("Do you want to read the accessibility statement?")
		iconSource: "qrc:/images/desktop/material_a11y.svg"

		//: LABEL DESKTOP
		title: qsTr("Accessibility statement")

		onClicked: Qt.openUrlExternally(buttonTooltip)
	}
	GSeparator {
		Layout.fillWidth: true
	}
	MoreViewMenuItem {
		Layout.fillWidth: true
		//: LABEL DESKTOP
		buttonText: qsTr("Start setup assistant")
		//: LABEL DESKTOP
		description: qsTr("Do you want to run the setup assistant again?")
		iconSource: "qrc:/images/desktop/material_assistant.svg"

		//: LABEL DESKTOP
		title: qsTr("Setup assistant")

		onClicked: sectionPage.nextView(UiModule.TUTORIAL)
	}
}
