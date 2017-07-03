import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0

import "../"
import "../global"


SectionPage {
	id: root
	headerTitleBarAction: TitleBarAction { text: qsTr("Dialog & Feedback"); font.bold: true }

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
				text: qsTr("Your opinion matters")
				font.pixelSize: Constants.header_font_size
				color: Constants.blue
				wrapMode: Text.WordWrap
			}
			Text {
				id: subtitle
				anchors.left: parent.left
				anchors.right: parent.right
				text: qsTr("We are happy about every feedback on our software.")
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			Pane {
				Loader {
					readonly property string titleText: qsTr("Rate AusweisApp2")
					readonly property string descriptionText: qsTr("Please rate us on the Google Play Store.")
					function onClickFunction() { Qt.openUrlExternally("market://details?id=com.governikus.ausweisapp2") }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Share")
					readonly property string descriptionText: qsTr("Tell your friends about AusweisApp2.")
					readonly property string shareTitle: qsTr("Share with")
					readonly property string share: qsTr("I'm using AusweisApp2, download it here for Android: " +
														 "https://play.google.com/store/apps/details?id=com.governikus.ausweisapp2")
					function onClickFunction() { qmlExtension.shareText(share, shareTitle) }
					width: parent.width
					sourceComponent: subMenu
				}
				Loader { width: parent.width; sourceComponent: lineSeparator }
				Loader {
					readonly property string titleText: qsTr("Report error")
					readonly property string descriptionText: qsTr("You found a bug? Please tell us, so we can fix it.")
					readonly property string emailAddress: "support.ausweisapp2@governikus.de"
					readonly property string emailSubject: qsTr("Android log file")
					readonly property string emailBody: qsTr("<Please describe the error>")
					function onClickFunction() { qmlExtension.mailLog(emailAddress, emailSubject, emailBody) }
					width: parent.width
					sourceComponent: subMenu
				}
			}
		}
	}
}
