/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.HistoryView 1.0
import Governikus.DeveloperView 1.0
import Governikus.TutorialView 1.0
import Governikus.View 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage {
	id: moreView

	//: LABEL ANDROID IOS DESKTOP_QML
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
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Version information") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_npa.svg"
			showRightArrow: true
			onClicked: firePush(versionInformationPage)
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Tutorial") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_tutorial.svg"
			showRightArrow: true
			onClicked: firePush(tutorialView)
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("FAQ") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_info.svg"
			//: LABEL ANDROID IOS DESKTOP_QML
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/"))
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Support") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_fragen.svg"
			//: LABEL ANDROID IOS DESKTOP_QML
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/"))
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Rate app") + SettingsModel.translationTrigger;
			icon: "qrc:///images/iOS/more/icon_mehr_favorit.svg";
			onClicked: {
				let ratingUrl = Qt.platform.os === "android" ? "market://details?id=com.governikus.ausweisapp2"
					: "itms-apps:itunes.apple.com/%1/app/ausweisapp2/id948660805?mt=8&action=write-review".arg(SettingsModel.language)
				Qt.openUrlExternally(ratingUrl)
			}
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Report error") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_report.svg"
			onClicked: LogModel.mailLog()
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Software license") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_license.svg"
			onClicked: {
				if (Qt.platform.os === "android") {
					//: LABEL ANDROID IOS DESKTOP_QML
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
				}
				else if (Qt.platform.os === "ios") {
					//: LABEL ANDROID IOS DESKTOP_QML
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/ios/"))
				}
				else {
					//: LABEL ANDROID IOS DESKTOP_QML
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/"))
				}
			}
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("History") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_verlauf.svg"
			showRightArrow: true
			onClicked: {
				firePush(historyView)
			}
		}

		MoreViewMenuItem {
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Show log") + SettingsModel.translationTrigger
			icon: "qrc:///images/iOS/more/icon_mehr_log.svg"
			showRightArrow: true
			showSeparator: plugin.developerBuild
			onClicked: firePush(logView)
		}

		MoreViewMenuItem {
			visible: plugin.developerBuild
			//: LABEL ANDROID IOS DESKTOP_QML
			text: qsTr("Developer options") + SettingsModel.translationTrigger
			icon: "qrc:///images/zahnraeder.svg"
			showRightArrow: true
			showSeparator: false
			onClicked: firePush(developerView)
		}

		GSeparator {
			Layout.bottomMargin: 2 * Constants.component_spacing + languageButtons.childrenRect.height
			Layout.fillWidth: true
		}
	}

	Item {
		id: languageButtons

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		LocationButton {
			id: lang_de

			anchors.margins: Constants.component_spacing
			anchors.bottom: parent.bottom
			anchors.right: lang_en.left

			Accessible.name: qsTr("Set language to german") + SettingsModel.translationTrigger

			language: "de"
			name: "DE"
			image: "qrc:///images/location_flag_de.svg"
		}

		LocationButton {
			id: lang_en

			anchors.margins: Constants.component_spacing
			anchors.bottom: parent.bottom
			anchors.right: parent.right

			Accessible.name: qsTr("Set language to english") + SettingsModel.translationTrigger

			language: "en"
			name: "EN"
			image: "qrc:///images/location_flag_en.svg"
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
		id: developerView
		DeveloperView {}
	}

	Component {
		id: logView
		LogView {}
	}

	Component {
		id: tutorialView
		TutorialView {}
	}
}
