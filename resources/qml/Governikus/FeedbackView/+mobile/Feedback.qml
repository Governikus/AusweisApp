import QtQuick 2.10
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.LogModel 1.0

import Governikus.Type.ApplicationModel 1.0


SectionPage {
	id: root
	headerTitleBarAction: TitleBarAction { text: qsTr("Help & Feedback") + settingsModel.translationTrigger; font.bold: true }

	Component {
		id: lineSeparator
		Rectangle {
			height: 1
			color: Constants.grey
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
				Text {
					width: parent.width
					font.pixelSize: Utils.dp(18)
					color: Constants.blue
					wrapMode: Text.WordWrap
					text: titleText
				}
				Text {
					color: Constants.secondary_text
					width: parent.width
					font.pixelSize: Constants.normal_font_size
					wrapMode: Text.WordWrap
					text: descriptionText
				}
			}
			MouseArea {
				anchors.fill: parent
				onClicked: onClickFunction()
			}
		}
	}
	Log {
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
					readonly property string titleText: qsTr("FAQ") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Do you have questions how to use AusweisApp2?") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/frequently-asked-questions/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Support") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("You need further help?") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/qa/support/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Rate AusweisApp2") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Please rate us in the Google Play Store.") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally("market://details?id=" + ApplicationModel.packageName) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Report error") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("You found a bug? Please tell us, so we can fix it.") + settingsModel.translationTrigger
					function onClickFunction() { LogModel.mailLog() }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Show log") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("You can view the logs of the AusweisApp2 here.") + settingsModel.translationTrigger
					function onClickFunction() { firePush(logPage) }
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
