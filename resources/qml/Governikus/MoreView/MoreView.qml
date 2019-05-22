import QtQuick 2.10
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.DeveloperView 1.0
import Governikus.TutorialView 1.0
import Governikus.View 1.0
import Governikus.Type.RemoteServiceModel 1.0

SectionPage {
	headerTitleBarAction: TitleBarAction { text: qsTr("More") + settingsModel.translationTrigger; font.bold: true }

	Rectangle {
		anchors.fill: menu
		color: "white"
	}

	Column {
		id: menu
		width: parent.width
		anchors.top: parent.top
		anchors.topMargin: Constants.component_spacing

		MoreViewMenuItem {
			text: qsTr("Version information") + settingsModel.translationTrigger
			imageSource: "qrc:///images/npa.svg"
			showRightArrow: true
			onClicked: firePush(versionInformationPage)
		}

		MoreViewMenuItem {
			text: qsTr("Tutorial") + settingsModel.translationTrigger
			imageSource: "qrc:///images/iOS/more/icon_mehr_tutorial.svg"
			showRightArrow: true
			onClicked: firePush(tutorialView)
		}

		MoreViewMenuItem {
			text: qsTr("FAQ") + settingsModel.translationTrigger
			imageSource: "qrc:///images/iOS/more/icon_mehr_info.svg"
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/"))
		}

		MoreViewMenuItem {
			text: qsTr("Support") + settingsModel.translationTrigger
			imageSource: "qrc:///images/iOS/more/icon_mehr_fragen.svg"
			onClicked: Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/"))
		}

		MoreViewMenuItem {
			text: qsTr("Rate app") + settingsModel.translationTrigger;
			imageSource: "qrc:///images/iOS/more/icon_mehr_favorit.svg";
			// Use Qt.platform.os here instead of platformstyle because rating market URLs on iOS doesn't work and vice versa
			onClicked: {
				if (Qt.platform.os === "android") {
					Qt.openUrlExternally("market://details?id=com.governikus.ausweisapp2")
				}
				else if (Qt.platform.os === "ios") {
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/evaluate-us/"))
				}
			}
		}

		MoreViewMenuItem {
			text: qsTr("Software license") + settingsModel.translationTrigger
			imageSource: "qrc:///images/iOS/more/icon_mehr_license.svg"
			onClicked: {
				if (Qt.platform.os === "android") {
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
				}
				else if (Qt.platform.os === "ios") {
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/ios/"))
				}
				else {
					Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/"))
				}
			}
		}

		MoreViewMenuItem {
			imageSource: "qrc:///images/iOS/more/icon_mehr_remotereader"
			text: qsTr("Configure remote service") + settingsModel.translationTrigger
			showRightArrow: true
			onClicked: {
				firePush(remoteServiceView)
			}
		}

		MoreViewMenuItem {
			text: qsTr("Show log") + settingsModel.translationTrigger
			imageSource: "qrc:///images/iOS/more/icon_mehr_log.svg"
			showRightArrow: true
			onClicked: firePush(logView)
		}

		MoreViewMenuItem {
			visible: plugin.developerBuild
			text: qsTr("Developer options") + settingsModel.translationTrigger
			imageSource: "qrc:///images/zahnraeder.svg"
			showRightArrow: true
			onClicked: firePush(developerView)
		}
	}

	Item {
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		LocationButton {
			id: lang_de

			language: "de"
			name: "DE"
			image: "qrc:///images/location_flag_de.svg"

			anchors.margins: Constants.component_spacing
			anchors.bottom: parent.bottom
			anchors.right: lang_en.left
		}

		LocationButton {
			id: lang_en

			language: "en"
			name: "EN"
			image: "qrc:///images/location_flag_en.svg"

			anchors.margins: Constants.component_spacing
			anchors.bottom: parent.bottom
			anchors.right: parent.right
		}
	}

	Rectangle {
		anchors.top: menu.top
		height: 1; width: parent.width
		color: Constants.grey
	}
	Rectangle {
		anchors.bottom: menu.bottom
		height: 1; width: parent.width
		color: Constants.grey
	}

	VersionInformation {
		id: versionInformationPage
		visible: false
	}

	RemoteServiceView {
		id: remoteServiceView
		visible: false
	}

	DeveloperView {
		id: developerView
		visible: false
	}

	Log {
		id: logView
		visible: false
	}

	TutorialView {
		id: tutorialView
		visible: false
	}
}
