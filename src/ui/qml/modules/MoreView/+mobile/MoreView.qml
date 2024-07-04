/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type
import Governikus.SettingsView

FlickableSectionPage {
	id: baseItem

	enableTileStyle: false
	//: LABEL ANDROID IOS
	title: qsTr("Help")

	Column {
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GOptionsContainer {
			//: LABEL ANDROID IOS
			title: qsTr("General")
			width: parent.width

			GMenuItem {
				drawTopCorners: true
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("FAQ - Frequently asked questions")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/faq".arg(SettingsModel.language))
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Contact")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/support".arg(SettingsModel.language))
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Privacy statement")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language))
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Accessibility statement")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/a11y".arg(SettingsModel.language))
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("List of Providers")
				width: parent.width

				onClicked: Qt.openUrlExternally("https://www.ausweisapp.bund.de/%1/aa2/providerlist".arg(SettingsModel.language))
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				drawBottomCorners: true
				icon.source: "qrc:///images/open_website.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Rate %1").arg(Qt.application.name)
				width: parent.width

				onClicked: Qt.openUrlExternally(ApplicationModel.storeUrl)
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
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

				onClicked: push(logPage)
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)

				Component {
					id: logPage

					LogView {
						enableTileStyle: baseItem.enableTileStyle
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				drawBottomCorners: true
				icon.source: "qrc:///images/email_icon.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Send log to the support")
				width: parent.width

				onClicked: LogModel.mailLog()
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
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

				onClicked: push(versionInformation)
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)

				Component {
					id: versionInformation

					VersionInformation {
						enableTileStyle: baseItem.enableTileStyle
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				//: LABEL ANDROID IOS
				title: qsTr("Terms of use and software license")
				width: parent.width

				onClicked: push(licenseInformation)
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)

				Component {
					id: licenseInformation

					LicenseInformation {
						enableTileStyle: baseItem.enableTileStyle
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GMenuItem {
				drawBottomCorners: true
				//: LABEL ANDROID IOS
				title: qsTr("Release notes")
				width: parent.width

				onClicked: push(releaseNotes)
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)

				Component {
					id: releaseNotes

					ReleaseNotes {
						enableTileStyle: baseItem.enableTileStyle
					}
				}
			}
		}
	}
}
