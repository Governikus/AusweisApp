import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.VersionInformationView 1.0


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
					font.pixelSize: Utils.sp(18)
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

			Text {
				id: title
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("You need help?") + settingsModel.translationTrigger
				font.pixelSize: Constants.header_font_size
				color: Constants.blue
				wrapMode: Text.WordWrap
			}
			Text {
				id: subtitle
				color: Constants.secondary_text
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Here you are in the right place.") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			Pane {
				Loader {
					readonly property string titleText: qsTr("Version information") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Here you can see detailed information about AusweisApp2.") + settingsModel.translationTrigger
					function onClickFunction() { firePush(versionInformationPage, {}) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Software license") + settingsModel.translationTrigger
					readonly property string descriptionText: qsTr("Read the software license text on the application homepage.") + settingsModel.translationTrigger
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/")) }
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
