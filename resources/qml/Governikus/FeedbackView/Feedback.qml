import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import Governikus.Global 1.0
import Governikus.TitleBar 1.0


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
					font.pixelSize: Utils.sp(18)
					color: Constants.blue
					wrapMode: Text.WordWrap
					text: titleText
				}
				Text {
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

	content: Item {
		width: root.width
		height: childrenRect.height

		Column {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: Constants.component_spacing
			spacing: Constants.component_spacing
			padding: Constants.component_spacing

			Text {
				id: title
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Your opinion matters") + settingsModel.translationTrigger
				font.pixelSize: Constants.header_font_size
				color: Constants.blue
				wrapMode: Text.WordWrap
			}
			Text {
				id: subtitle
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("We are happy about every feedback on our software.") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			Pane {
				Loader {
					readonly property string titleText: qsTr("FAQ") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Do you have questions how to use AusweisApp2?") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/questions-and-answers/frequently-asked-questions/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Support") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("You need further help?") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/questions-and-answers/support/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Rate AusweisApp2") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Please rate us in the Google Play Store.") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally("market://details?id=com.governikus.ausweisapp2") }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Report error") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("You found a bug? Please tell us, so we can fix it.") + settingsModel.translationTrigger
					readonly property string emailAddress: "support.ausweisapp2@governikus.de"
					readonly property string emailSubject: qsTr("Android log file") + settingsModel.translationTrigger
					readonly property string emailBody: qsTr("<Please describe the error>") + settingsModel.translationTrigger
					function onClickFunction() { qmlExtension.mailLog(emailAddress, emailSubject, emailBody) }
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
