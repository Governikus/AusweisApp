/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("FAQ") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you have questions how to use %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_questions.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		buttonTooltip: qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/") + SettingsModel.translationTrigger
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/")) + SettingsModel.translationTrigger
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Support") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you need further help?") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/help_icon.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		buttonTooltip: qsTr("https://www.ausweisapp.bund.de/en/qa/support/") + SettingsModel.translationTrigger
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/")) + SettingsModel.translationTrigger
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Report error") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Did you find a bug? Please tell us, so we can fix it.") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_report_error.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		buttonTooltip: qsTr("https://www.ausweisapp.bund.de/en/qa/report-an-error/") + SettingsModel.translationTrigger
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/report-an-error/")) + SettingsModel.translationTrigger
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Rate %1").arg(Qt.application.name) + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to rate the %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_rate_application.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		buttonTooltip: qsTr("https://www.ausweisapp.bund.de/en/qa/evaluate-us/") + SettingsModel.translationTrigger
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/evaluate-us/")) + SettingsModel.translationTrigger
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Online help") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you need further help?") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_manual.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		buttonTooltip: qsTr("Help") + SettingsModel.translationTrigger
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: ApplicationModel.openOnlineHelp("index")
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Software licenses") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to read about the software licenses?") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_license.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		buttonTooltip: qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/") + SettingsModel.translationTrigger
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/")) + SettingsModel.translationTrigger
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Release notes") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to read the release notes?") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/main_info.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Open website") + SettingsModel.translationTrigger
		buttonTooltip: ApplicationModel.releaseNotesUrl
		buttonIconSource: "qrc:///images/link_external.svg"
		onClicked: Qt.openUrlExternally(ApplicationModel.releaseNotesUrl)
	}

	GSeparator {
		Layout.fillWidth: true
	}

	MoreViewMenuItem {
		Layout.fillWidth: true

		//: LABEL DESKTOP_QML
		title: qsTr("Setup assistant") + SettingsModel.translationTrigger
		//: LABEL DESKTOP_QML
		description: qsTr("Do you want to run the setup assistant again?") + SettingsModel.translationTrigger
		iconSource: "qrc:/images/desktop/info_setup.svg"
		//: LABEL DESKTOP_QML
		buttonText: qsTr("Start setup assistant") + SettingsModel.translationTrigger
		onClicked: sectionPage.nextView(SectionPage.Views.SetupAssistant)
	}
}
