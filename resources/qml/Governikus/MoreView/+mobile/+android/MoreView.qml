/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.TutorialView 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage {
	id: root

	//: LABEL ANDROID IOS
	title: qsTr("Help & Feedback") + SettingsModel.translationTrigger

	content: ColumnLayout {
		width: root.width

		spacing: 0

		MoreViewMenuItem {
			//: LABEL ANDROID
			text: qsTr("Tutorial") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you want to know how to use %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
			onClicked: firePush(tutorialPage)
		}

		MoreViewLinkMenuItem {
			//: LABEL ANDROID
			text: qsTr("FAQ") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you have further questions about %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/"))
		}

		MoreViewLinkMenuItem {
			//: LABEL ANDROID
			text: qsTr("Support") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you need further support?") + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/"))
		}

		MoreViewLinkMenuItem {
			//: LABEL ANDROID
			text: qsTr("Rate AusweisApp2") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you want to rate us in the Google Play Store?") + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally("market://details?id=" + ApplicationModel.packageName)
		}

		MoreViewLinkMenuItem {
			//: LABEL ANDROID
			text: qsTr("Release notes") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you want to view the release notes of %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
		}

		MoreViewMenuItem {
			//: LABEL ANDROID
			text: qsTr("Report error") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Did you find a bug? Please tell us, so we can fix it.") + SettingsModel.translationTrigger
			onClicked: LogModel.mailLog()
		}

		MoreViewMenuItem {
			//: LABEL ANDROID
			text: qsTr("Show log") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you want to view the logs of %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
			onClicked: firePush(logPage)
		}

		MoreViewMenuItem {
			//: LABEL ANDROID
			text: qsTr("Version information") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you want to see detailed information about %1?").arg(Qt.application.name) + SettingsModel.translationTrigger
			onClicked: firePush(versionInformationPage)
		}

		MoreViewLinkMenuItem {
			//: LABEL ANDROID
			text: qsTr("Software license") + SettingsModel.translationTrigger
			//: LABEL ANDROID
			footerText: qsTr("Do you want to read the software licenses?") + SettingsModel.translationTrigger
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
		}
	}

	Component {
		id: logPage

		LogView {}
	}

	Component {
		id: tutorialPage

		TutorialView {}
	}

	Component {
		id: versionInformationPage

		VersionInformation {}
	}
}
