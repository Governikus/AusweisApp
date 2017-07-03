import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.2

import "../"
import "../global"


Item {
	id: header
	/* this is interpreted by the SectionPage component */
	readonly property real titleBarOpacity: shadow.opacity === 1 ? 1 : 0
	readonly property int stopFlickOn: Constants.titlebar_height

	Image {
		id: dna
		width: parent.width
		source: "qrc:///images/iOS/Header-Ausweisapp@3x.png"
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.bottomMargin: (parent.height - 2 * Constants.titlebar_height) * transition()

		function transition() {
			return Math.min(1, contentY / Constants.titlebar_height)
		}

		Rectangle {
			id: shadow
			anchors.fill: parent
			color: Constants.blue
			opacity: Math.min(1, 0.5 + parent.transition() * 0.5)
			Behavior on opacity {
				NumberAnimation {}
			}
		}
	}

	Item {
		id: information
		height: Math.max(npa.height, text.height)
		anchors.margins: Utils.dp(15)
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		Image {
			id: npa
			anchors.left: parent.left
			anchors.verticalCenter: information.verticalCenter

			height: Utils.dp(60)
			width: height
			source: "qrc:///images/npa.ico"
		}
		Text {
			id: text
			anchors.verticalCenter: npa.verticalCenter
			anchors.left: npa.right
			anchors.leftMargin: Utils.dp(15)
			anchors.right: parent.right

			text: (applicationModel.currentWorkflow !== "authentication") ?
					qsTr("Hello, here you have the opportunity to view the stored data on your identity card.") :
					qsTr("Hello, \"%1\", wants to read your data").arg(certificateDescriptionModel.subjectName)
			wrapMode: Text.WordWrap
			font.pixelSize: Constants.normal_font_size
		}
	}
}
