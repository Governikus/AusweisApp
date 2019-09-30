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
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage {
	id: root
	//: LABEL ANDROID IOS
	title: qsTr("Help & Feedback") + SettingsModel.translationTrigger

	Component {
		id: lineSeparator
		GSeparator {
		}
	}
	Component {
		id: subMenu
		Item {
			height: column.height
			Column {
				id: column
				anchors.left: parent.left
				anchors.right: parent.right
				spacing: Constants.component_spacing
				GText {
					width: parent.width
					text: titleText
					textStyle: Style.text.normal_accent
				}
				GText {
					width: parent.width
					text: descriptionText
				}
			}
			MouseArea {
				anchors.fill: parent
				onClicked: onClickFunction()
			}
		}
	}
	LogView {
		id: logPage
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
					readonly property string titleText: qsTr("FAQ") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("Do you have questions how to use AusweisApp2?") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					//: LABEL ANDROID IOS
					readonly property string titleText: qsTr("Support") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("You need further help?") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					//: LABEL ANDROID IOS
					readonly property string titleText: qsTr("Rate AusweisApp2") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("Please rate us in the Google Play Store.") + SettingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally("market://details?id=" + ApplicationModel.packageName) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					//: LABEL ANDROID IOS
					readonly property string titleText: qsTr("Report error") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("You found a bug? Please tell us, so we can fix it.") + SettingsModel.translationTrigger
					function onClickFunction() { LogModel.mailLog() }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					//: LABEL ANDROID IOS
					readonly property string titleText: qsTr("Show log") + SettingsModel.translationTrigger
					//: LABEL ANDROID IOS
					readonly property string descriptionText: qsTr("You can view the logs of the AusweisApp2 here.") + SettingsModel.translationTrigger
					function onClickFunction() { firePush(logPage) }
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
