/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
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
	//: MOBILE
	title: qsTr("Help")

	Column {
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing

		GOptionsContainer {
			//: MOBILE
			title: qsTr("General")
			width: parent.width

			GMenuItem {
				Layout.fillWidth: true
				drawTopCorners: true
				icon.source: "qrc:///images/npa.svg"
				tintIcon: false
				//: MOBILE
				title: qsTr("Start setup")

				onClicked: root.startOnboarding()
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				icon.source: "qrc:///images/open_website.svg"
				linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language)
				//: MOBILE
				title: qsTr("FAQ - Frequently asked questions")
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				icon.source: "qrc:///images/open_website.svg"
				linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language)
				//: MOBILE
				title: qsTr("Contact")
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				icon.source: "qrc:///images/open_website.svg"
				linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)
				//: MOBILE
				title: qsTr("Privacy statement")
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				icon.source: "qrc:///images/open_website.svg"
				linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language)
				//: MOBILE
				title: qsTr("Accessibility statement")
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				icon.source: "qrc:///images/open_website.svg"
				linkToOpen: "https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language)
				//: MOBILE
				title: qsTr("List of Providers")
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				drawBottomCorners: true
				icon.source: "qrc:///images/open_website.svg"
				linkToOpen: ApplicationModel.storeUrl
				//: MOBILE
				title: qsTr("Rate %1").arg(Qt.application.name)
			}
		}
		GOptionsContainer {
			//: MOBILE
			title: qsTr("Logs")
			width: parent.width

			GMenuItem {
				id: menuItemShowLogs

				Layout.fillWidth: true
				drawTopCorners: true
				//: MOBILE
				title: qsTr("Show Logs")

				onClicked: LogFilesModel.count > 1 ? root.push(logFilesView) : root.push(logView, {
					logFileName: LogFilesModel.getLogFileName(0),
					logFilePath: LogFilesModel.getLogFilePath(0)
				})

				Component {
					id: logFilesView

					LogFilesView {
						enableTileStyle: root.enableTileStyle

						onLogFilesListItemClicked: index => {
							root.push(logView, {
								logFileName: LogFilesModel.getLogFileName(index),
								logFilePath: LogFilesModel.getLogFilePath(index)
							});
						}
					}
				}
				Component {
					id: logView

					LogView {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				drawBottomCorners: true
				icon.source: "qrc:///images/email_icon.svg"
				//: MOBILE
				title: qsTr("Send log to the support")

				onClicked: logModel.mailLogFile()

				LogModel {
					id: logModel

				}
			}
		}
		GOptionsContainer {
			//: MOBILE
			title: qsTr("Information")
			width: parent.width

			GMenuItem {
				Layout.fillWidth: true
				drawTopCorners: true
				//: MOBILE %1 is replaced with the application name
				title: qsTr("%1 version").arg(Qt.application.name)

				onClicked: root.push(versionInformation)

				Component {
					id: versionInformation

					VersionInformation {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				//: MOBILE
				title: qsTr("Terms of use and software license")

				onClicked: root.push(licenseInformation)

				Component {
					id: licenseInformation

					LicenseInformation {
						enableTileStyle: root.enableTileStyle
					}
				}
			}
			MoreViewSeparator {
			}
			GMenuItem {
				Layout.fillWidth: true
				drawBottomCorners: true
				//: MOBILE
				title: qsTr("Release notes")

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

	component MoreViewSeparator: GSeparator {
		Layout.fillWidth: true
		Layout.leftMargin: Style.dimens.pane_spacing
		Layout.rightMargin: Layout.leftMargin
	}
}
