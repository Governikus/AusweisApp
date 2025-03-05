/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.View
import Governikus.FeedbackView
import Governikus.InformationView
import Governikus.Type
import Governikus.Style

FlickableSectionPage {
	id: root

	signal startOnboarding

	enableTileStyle: false
	//: LABEL ANDROID IOS
	title: qsTr("Help")

	Column {
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing

		GOptionsContainer {
			//: LABEL ANDROID IOS
			title: qsTr("General")
			width: parent.width

			GMenuItem {
				drawTopCorners: true
				icon.source: "qrc:///images/npa.svg"
				tintIcon: false
				//: LABEL ANDROID IOS
				title: qsTr("Start onboarding")
				width: parent.width

				onClicked: root.startOnboarding()
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("FAQ - Frequently asked questions")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Contact")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Privacy statement")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Accessibility statement")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("List of Providers")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language))
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				drawBottomCorners: true
				icon.source: "qrc:///images/open_website.svg"
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

			GMenuItem {
				drawTopCorners: true
				//: LABEL ANDROID IOS
				title: qsTr("Show Logs")
				width: parent.width

				onClicked: root.push(logPage)

				Component {
					id: logPage

					LogView {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				drawBottomCorners: true
				icon.source: "qrc:///images/email_icon.svg"
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

			GMenuItem {
				drawTopCorners: true
				//: LABEL ANDROID IOS
				title: qsTr("Version information")
				width: parent.width

				onClicked: root.push(versionInformation)

				Component {
					id: versionInformation

					VersionInformation {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				//: LABEL ANDROID IOS
				title: qsTr("Terms of use and software license")
				width: parent.width

				onClicked: root.push(licenseInformation)

				Component {
					id: licenseInformation

					LicenseInformation {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_spacing
				anchors.right: parent.right
				anchors.rightMargin: Style.dimens.pane_spacing
			}
			GMenuItem {
				drawBottomCorners: true
				//: LABEL ANDROID IOS
				title: qsTr("Release notes")
				width: parent.width

				onClicked: root.push(releaseNotes)

				Component {
					id: releaseNotes

					ReleaseNotes {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
		}
	}
}
