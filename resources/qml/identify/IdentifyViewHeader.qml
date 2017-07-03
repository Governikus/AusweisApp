import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import "../"
import "../global"


Rectangle {

	width: parent.width
	height: parent.height

	color: Constants.background_color

	Image {
		id: dna
		width: parent.width
		height: parent.height * 0.7
		source: "qrc:///images/iOS/Header-Ausweisapp@3x.png"
		fillMode: Image.PreserveAspectCrop
	}

	Item {
		id: information
		anchors.left: dna.left
		anchors.bottom: dna.bottom

		width: parent.width
		height: parent.height * 0.2

		Image {
			id: npaIco
			anchors.left: information.left
			anchors.leftMargin: Utils.dp(15)

			height: Utils.dp(50)
			width: height
			source: "qrc:///images/npa.svg"
			fillMode: Image.Stretch
		}

		Text {
			id: title

			anchors.top: npaIco.top
			anchors.left: npaIco.right
			anchors.leftMargin: Utils.dp(10)
			anchors.right: parent.right
			anchors.rightMargin: Utils.dp(5)

			text: applicationModel.currentWorkflow !== "authentication" ? qsTr("Hello, here you have the") : qsTr("Hello,")
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
		}

		Text {
			id: description

			anchors.top: title.bottom
			anchors.topMargin: Utils.dp(5)
			anchors.left: npaIco.right
			anchors.leftMargin: Utils.dp(10)
			anchors.right: parent.right
			anchors.rightMargin: Utils.dp(5)

			text: applicationModel.currentWorkflow !== "authentication" ?
					  qsTr("opportunity to view the stored data on your identity card.") :
					  qsTr("\"%1\"<br>wants to read your data").arg(certificateDescriptionModel.subjectName)
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
		}
	}
}
