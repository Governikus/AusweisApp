/*
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.TutorialView 1.0
import Governikus.FeedbackView 1.0
import Governikus.InformationView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.SettingsView 1.0
import Governikus.HistoryView 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem

	//: LABEL ANDROID IOS
	title: qsTr("Help")

	content: Column {
		width: baseItem.width
		spacing: 0

		TitledSeparator {
			width: parent.width
			contentMarginTop: Constants.component_spacing
			//: LABEL ANDROID IOS
			title: qsTr("Help & Feedback")
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Tutorial")
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to know how to use %1?").arg(Qt.application.name)
			onClicked: firePush(tutorialView)

			Component {
				id: tutorialView

				TutorialView {
					onLeave: {
						firePop()
						navBar.show(UiModule.HELP)
					}
				}
			}
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("FAQ")
			//: LABEL ANDROID IOS
			description: qsTr("Do you have further questions about %1?").arg(Qt.application.name)
			icon: "qrc:///images/material_open_in_new.svg"
			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language))
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Support")
			//: LABEL ANDROID IOS
			description: qsTr("Do you need further support?")
			icon: "qrc:///images/material_open_in_new.svg"
			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Rate %1").arg(Qt.application.name)
			//: LABEL ANDROID IOS
			description: Constants.is_layout_ios ?
				qsTr("Do you want to rate us in the App Store?") :
				qsTr("Do you want to rate us in the Google Play Store?")
			icon: "qrc:///images/material_open_in_new.svg"
			onClicked: Qt.openUrlExternally(ApplicationModel.storeUrl)
		}

		TitledSeparator {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Diagnosis")
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Logs")
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to view the logs of %1?").arg(Qt.application.name)
			onClicked: firePush(logPage)

			Component {
				id: logPage
				LogView {}
			}
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Report error")
			//: LABEL ANDROID IOS
			description: qsTr("Did you find a bug? Please help us by sending us the log file together with a description of the error.")
			icon: "qrc:///images/material_mail.svg"
			onClicked: LogModel.mailLog()
		}

		TitledSeparator {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Information")
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Version information")
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to see detailed information about %1?").arg(Qt.application.name)
			onClicked: firePush(versionInformation)

			Component {
				id: versionInformation
				VersionInformation {}
			}
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Software license")
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to read the software licenses?")
			onClicked: firePush(licenseInformation)

			Component {
				id: licenseInformation
				LicenseInformation {}
			}
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Release notes")
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to view the release notes of %1?").arg(Qt.application.name)
			onClicked: firePush(releaseNotes)

			Component {
				id: releaseNotes
				ReleaseNotes {}
			}
		}
	}
}
