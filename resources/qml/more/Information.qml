import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import "../"
import "../global"


SectionPage {
	id: root
	headerTitleBarAction: TitleBarAction { text: qsTr("Information & Help"); font.bold: true }

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
				text: qsTr("You need help?")
				font.pixelSize: Constants.header_font_size
				color: Constants.blue
				wrapMode: Text.WordWrap
			}
			Text {
				id: subtitle
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("Here you are in the right place.")
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			Pane {
				Loader {
					readonly property string titleText: qsTr("FAQ")
					readonly property string descriptionText: qsTr("Do you have questions how to use AusweisApp2?")
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/service/haeufig-gestellte-fragen/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Support")
					readonly property string descriptionText: qsTr("You need further help?")
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/service/support/")) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Version information")
					readonly property string descriptionText: qsTr("Here you can see detailed information about AusweisApp2.")
					function onClickFunction() { push(versionInformationPage) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Software license")
					readonly property string descriptionText: qsTr("Read the software license text on the application homepage.")
					function onClickFunction() { Qt.openUrlExternally(qsTr("https://www.ausweisapp.bund.de/en/download/")) }
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
