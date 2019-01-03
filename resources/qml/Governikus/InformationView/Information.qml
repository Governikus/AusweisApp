import QtQuick 2.10
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0

SectionPage {
	id: root
	headerTitleBarAction: TitleBarAction { text: qsTr("Information") + settingsModel.translationTrigger; font.bold: true }

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
					readonly property string titleText: qsTr("Version information") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Here you can see detailed information about AusweisApp2.") + settingsModel.translationTrigger
					function onClickFunction() { firePush(versionInformationPage) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Software license") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Read the software license text on the application homepage.") + settingsModel.translationTrigger
					function onClickFunction() {
							if (Qt.platform.os === "android") {
								Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/android/"))
							}
							else if (Qt.platform.os === "ios") {
								Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/ios/"))
							}
							else {
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
