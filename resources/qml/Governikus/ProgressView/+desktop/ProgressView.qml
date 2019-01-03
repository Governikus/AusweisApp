import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


SectionPage
{
	id: baseItem
	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	StatusIcon {
		id: circle
		height: ApplicationModel.scaleFactor * 400
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4
		busy: true
		source: "qrc:///images/desktop/sandglass.svg"
	}

	Text {
		id: text
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter
		width: parent.width - (2 * Constants.pane_padding)

		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		color: Constants.white
		onLinkActivated: Qt.openUrlExternally(link)
	}

	Text {
		id: subText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: text.bottom
		width: parent.width - (2 * Constants.pane_padding)

		font.pixelSize: Constants.header_font_size
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		color: Constants.secondary_text
		onLinkActivated: Qt.openUrlExternally(link)
	}

	Text {
		id: progressText
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.normal_font_size
		anchors.bottom: progressBar.top
		anchors.bottomMargin: ApplicationModel.scaleFactor * 20
		anchors.horizontalCenter: parent.horizontalCenter
		width: parent.width - (2 * Constants.pane_padding)
		wrapMode: Text.WordWrap
		color: Constants.white
	}

	ProgressBar {
		id: progressBar
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: ApplicationModel.scaleFactor * 80
		height: ApplicationModel.scaleFactor * 40
		from: 0
		to: 5
		visible: false
		value: progressValue

		background: Rectangle {
			radius: ApplicationModel.scaleFactor * 8
			color: Constants.lightgrey
		}

		contentItem: Item {
			Rectangle {
				width: progressBar.visualPosition * parent.width
				height: parent.height
				radius: ApplicationModel.scaleFactor * 8
				color: Constants.green
			}
		}
	}
}
