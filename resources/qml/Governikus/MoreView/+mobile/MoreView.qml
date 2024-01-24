/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.FeedbackView
import Governikus.InformationView
import Governikus.Type.ApplicationModel
import Governikus.Type.LogModel
import Governikus.Type.SettingsModel
import Governikus.SettingsView
import Governikus.Type.UiModule

FlickableSectionPage {
	id: baseItem

	//: LABEL ANDROID IOS
	title: qsTr("Help")

	Column {
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GOptionsContainer {
			//: LABEL ANDROID IOS
			title: qsTr("Help & Feedback")
			width: parent.width

			MenuItem {
				drawTopCorners: true
				icon: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("FAQ - Frequently asked questions")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				icon: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Contact")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				icon: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Privacy statement")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				icon: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Accessibility statement")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				icon: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("List of Providers")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				drawBottomCorners: true
				icon: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Rate %1").arg(Qt.application.name)
				width: parent.width

				onClicked: Qt.openUrlExternally(ApplicationModel.storeUrl)
			}
		}
		GOptionsContainer {
			//: LABEL ANDROID IOS
			title: qsTr("Logs")
			width: parent.width

			MenuItem {
				drawTopCorners: true
				//: LABEL ANDROID IOS
				title: qsTr("Show Logs")
				width: parent.width

				onClicked: push(logPage)

				Component {
					id: logPage

					LogView {
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				drawBottomCorners: true
				icon: "qrc:///images/material_mail.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Send log to the support")
				width: parent.width

				onClicked: LogModel.mailLog()
			}
		}
		GOptionsContainer {
			//: LABEL ANDROID IOS
			title: qsTr("Information")
			width: parent.width

			MenuItem {
				drawTopCorners: true
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
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				//: LABEL ANDROID IOS
				title: qsTr("Terms of use and software license")
				width: parent.width

				onClicked: push(licenseInformation)

				Component {
					id: licenseInformation

					LicenseInformation {
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				drawBottomCorners: true
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
}
