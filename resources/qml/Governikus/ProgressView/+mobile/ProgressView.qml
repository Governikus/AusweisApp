import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0

SectionPage
{
	id: baseItem
	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color
	property alias progressText: progressText.text
	property int progressValue
	property alias progressBarVisible: progressBar.visible

	BusyIndicator {
		id: busyIndicator
		anchors.fill: circle
		running: baseItem.visible
		contentItem: NpaBusyIndicatorStyle { factor: 1.2 }
	}
	Rectangle {
		id: circle
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: Constants.is_tablet ? parent.height / 4 : parent.width / 2
		width: parent.height / 4
		height: width
		radius: width / 2
		color: Constants.blue
	}
	Text {
		id: text
		anchors.top: circle.bottom
		anchors.topMargin: Utils.dp(50)
		anchors.horizontalCenter: parent.horizontalCenter
		font.pixelSize: Constants.header_font_size
		font.weight: Font.Bold
		color: Constants.blue
		horizontalAlignment: Text.AlignHCenter
		wrapMode: Text.WordWrap
		width: baseItem.width * 0.8
	}
	Text {
		id: subText
		color: Constants.secondary_text
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.normal_font_size
		anchors.top: text.bottom
		anchors.topMargin: Utils.dp(10)
		anchors.horizontalCenter: parent.horizontalCenter
		width: baseItem.width * 0.8
		wrapMode: Text.WordWrap
	}
	Text {
		id: progressText
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.normal_font_size
		anchors.top: subText.bottom
		anchors.topMargin: Utils.dp(20)
		anchors.horizontalCenter: parent.horizontalCenter
		width: baseItem.width * 0.8
		wrapMode: Text.WordWrap
		color: Constants.grey
	}
	ProgressBar {
		id: progressBar
		anchors.top: progressText.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.topMargin: Utils.dp(10)
		width: baseItem.width * 0.6
		height: Utils.dp(24)
		from: 0
		to: 5
		visible: false
		value: progressValue

		background: Rectangle {
			radius: Utils.dp(2)
			color: Constants.lightgrey
		}

		contentItem: Item {
			Rectangle {
				width: progressBar.visualPosition * parent.width
				height: parent.height
				radius: Utils.dp(2)
				color: Constants.green
			}
		}
	}
}
