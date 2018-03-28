import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0

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
		style: NpaBusyIndicatorStyle { factor: 1.2 }
	}
	Rectangle {
		id: circle
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: PlatformConstants.is_tablet ? parent.height / 4 : parent.width / 2
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
	}
	Text {
		id: subText
		color: Constants.secondary_text
		verticalAlignment: Text.AlignVCenter
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
		verticalAlignment: Text.AlignVCenter
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
		minimumValue: 0
		maximumValue: 5
		visible: false
		value: progressValue
		style: ProgressBarStyle {
			background: Rectangle {
				radius: Utils.dp(2)
				color: Constants.lightgrey
				implicitWidth: parent.width
				implicitHeight: parent.width/12.0
			}
			progress: Rectangle {
				radius: Utils.dp(2)
				color: Constants.green
			}
		}
	}
}
