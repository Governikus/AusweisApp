/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.HistoryView 1.0
import Governikus.TutorialView 1.0
import Governikus.View 1.0
import Governikus.SettingsView 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage {
	id: moreView

	//: LABEL IOS
	title: qsTr("More") + SettingsModel.translationTrigger

	content: ColumnLayout {
		id: menu

		width: moreView.width

		spacing: 0

		GSeparator {
			Layout.topMargin: Constants.component_spacing
			Layout.fillWidth: true
		}

		MoreViewMenuItem {
			//: LABEL IOS
			text: qsTr("Version information") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_npa.svg"
			onClicked: firePush(versionInformationPage)
		}

		MoreViewMenuItem {
			//: LABEL IOS
			text: qsTr("Settings") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_settings.svg"
			onClicked: firePush(settingsView)
		}

		MoreViewMenuItem {
			//: LABEL IOS
			text: qsTr("History") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_verlauf.svg"
			onClicked: {
				firePush(historyView)
			}
		}

		MoreViewMenuItem {
			//: LABEL IOS
			text: qsTr("Tutorial") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_tutorial.svg"
			onClicked: firePush(tutorialView)
		}

		MoreViewMenuItem {
			//: LABEL IOS
			text: qsTr("Show log") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_log.svg"
			onClicked: firePush(logView)
		}

		MoreViewLinkMenuItem {
			//: LABEL IOS
			text: qsTr("FAQ") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_info.svg"
			//: LABEL IOS
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/"))
		}

		MoreViewLinkMenuItem {
			//: LABEL IOS
			text: qsTr("Support") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_fragen.svg"
			//: LABEL IOS
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/"))
		}

		MoreViewLinkMenuItem {
			//: LABEL IOS
			text: qsTr("Rate app") + SettingsModel.translationTrigger;
			icon: "qrc:///images/iOS/more/icon_mehr_favorit.svg";
			onClicked: {
				let ratingUrl = Qt.platform.os === "android" ? "market://details?id=com.governikus.ausweisapp2"
					: "itms-apps:itunes.apple.com/%1/app/ausweisapp2/id948660805?mt=8&action=write-review".arg(SettingsModel.language)
				Qt.openUrlExternally(ratingUrl)
			}
		}

		MoreViewLinkMenuItem {
			//: LABEL IOS
			text: qsTr("Report error") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_report.svg"
			onClicked: LogModel.mailLog()
		}

		MoreViewLinkMenuItem {
			//: LABEL IOS
			text: qsTr("Release notes") + SettingsModel.translationTrigger;
			icon: "qrc:///images/iOS/more/icon_mehr_info.svg";
			onClicked: {
				if (Qt.platform.os === "android") {
					//: LABEL IOS
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
				}
				else if (Qt.platform.os === "ios") {
					//: LABEL IOS
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/ios/"))
				}
				else {
					//: LABEL IOS
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/"))
				}
			}
		}

		MoreViewLinkMenuItem {
			//: LABEL IOS
			text: qsTr("Software license") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_license.svg"
			showSeparator: false
			onClicked: {
				if (Qt.platform.os === "android") {
					//: LABEL IOS
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
				}
				else if (Qt.platform.os === "ios") {
					//: LABEL IOS
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/ios/"))
				}
				else {
					//: LABEL IOS
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/"))
				}
			}
		}

		GSeparator {
			Layout.bottomMargin: Constants.component_spacing
			Layout.fillWidth: true
		}
	}

	Component {
		id: versionInformationPage

		VersionInformation {}
	}

	Component {
		id: historyView

		HistoryView {}
	}

	Component {
		id: logView

		LogView {}
	}

	Component {
		id: tutorialView

		TutorialView {}
	}

	Component {
		id: settingsView

		SettingsView {}
	}
}
