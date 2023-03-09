/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
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
		spacing: 0
		width: baseItem.width

		TitledSeparator {
			contentMarginTop: Constants.component_spacing
			//: LABEL ANDROID IOS
			title: qsTr("Help & Feedback")
			width: parent.width
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to know how to use %1?").arg(Qt.application.name)
			//: LABEL ANDROID IOS
			title: qsTr("Tutorial")
			width: parent.width

			onClicked: push(tutorialView)

			Component {
				id: tutorialView
				TutorialView {
					Component.onDestruction: {
						show(UiModule.HELP);
					}
					onLeave: {
						pop();
					}
				}
			}
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to see the video tutorials?")
			icon: "qrc:///images/material_open_in_new.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Video tutorials")
			width: parent.width

			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/videotutorials".arg(SettingsModel.language))
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you have further questions about %1?").arg(Qt.application.name)
			icon: "qrc:///images/material_open_in_new.svg"
			//: LABEL ANDROID IOS
			title: qsTr("FAQ")
			width: parent.width

			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language))
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you need further support?")
			icon: "qrc:///images/material_open_in_new.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Support")
			width: parent.width

			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to read the privacy statement?")
			icon: "qrc:///images/material_open_in_new.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Privacy statement")
			width: parent.width

			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language))
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to read the accessibility statement?")
			icon: "qrc:///images/material_open_in_new.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Accessibility statement")
			width: parent.width

			onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language))
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: Constants.is_layout_ios ? qsTr("Do you want to rate us in the App Store?") : qsTr("Do you want to rate us in the Google Play Store?")
			icon: "qrc:///images/material_open_in_new.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Rate %1").arg(Qt.application.name)
			width: parent.width

			onClicked: Qt.openUrlExternally(ApplicationModel.storeUrl)
		}
		TitledSeparator {
			//: LABEL ANDROID IOS
			title: qsTr("Diagnosis")
			width: parent.width
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to view the logs of %1?").arg(Qt.application.name)
			//: LABEL ANDROID IOS
			title: qsTr("Logs")
			width: parent.width

			onClicked: push(logPage)

			Component {
				id: logPage
				LogView {
				}
			}
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Did you find a bug? Please help us by sending us the log file together with a description of the error.")
			icon: "qrc:///images/material_mail.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Report error")
			width: parent.width

			onClicked: LogModel.mailLog()
		}
		TitledSeparator {
			//: LABEL ANDROID IOS
			title: qsTr("Information")
			width: parent.width
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to see detailed information about %1?").arg(Qt.application.name)
			//: LABEL ANDROID IOS
			title: qsTr("Version information")
			width: parent.width

			onClicked: push(versionInformation)

			Component {
				id: versionInformation
				VersionInformation {
				}
			}
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to read the software licenses?")
			//: LABEL ANDROID IOS
			title: qsTr("Software license")
			width: parent.width

			onClicked: push(licenseInformation)

			Component {
				id: licenseInformation
				LicenseInformation {
				}
			}
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Do you want to view the release notes of %1?").arg(Qt.application.name)
			//: LABEL ANDROID IOS
			title: qsTr("Release notes")
			width: parent.width

			onClicked: push(releaseNotes)

			Component {
				id: releaseNotes
				ReleaseNotes {
				}
			}
		}
	}
}
