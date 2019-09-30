/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: root
	//: LABEL ANDROID IOS
	title: qsTr("Information") + SettingsModel.translationTrigger

	Component {
		id: subMenu
		Item {
			height: column.height

			Accessible.role: Accessible.Button
			Accessible.name: titleText + " " + descriptionText

			Column {
				id: column
				anchors.left: parent.left
				anchors.right: parent.right
				spacing: Constants.component_spacing
				GText {
					width: parent.width

					Accessible.ignored: true

					text: titleText
					textStyle: Style.text.normal_accent
				}
				GText {
					width: parent.width

					Accessible.ignored: true

					text: descriptionText
					textStyle: Style.text.normal
				}
			}
			MouseArea {
				anchors.fill: parent
				onClicked: onClickFunction()
			}
		}
	}
	VersionInformation {
		id: versionInformationPage
		visible: false
	}

	content: Item {
		width: root.width
		height: childrenRect.height

		Column {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Constants.component_spacing
			spacing: Constants.component_spacing
			padding: Constants.component_spacing

			Pane {
				Loader {
					//: LABEL ANDROID IOS
					readonly property string titleText: qsTr("Version information") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("Here you can see detailed information about AusweisApp2.") + SettingsModel.translationTrigger
					function onClickFunction() { firePush(versionInformationPage) }
					width: parent.width
					sourceComponent: subMenu
				}
				GSeparator { width: parent.width }
				Loader {
					//: LABEL ANDROID IOS
					readonly property string titleText: qsTr("Software license") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("Read the software license text on the application homepage.") + SettingsModel.translationTrigger
					function onClickFunction() {
							if (Qt.platform.os === "android") {
								//: LABEL ANDROID IOS
								Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
							}
							else if (Qt.platform.os === "ios") {
								//: LABEL ANDROID IOS
								Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/ios/"))
							}
							else {
								//: LABEL ANDROID IOS
								Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/windows-and-mac/"))
							}
					}
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
