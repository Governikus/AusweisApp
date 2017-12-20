import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import Governikus.Global 1.0
import Governikus.Style 1.0

SectionPage
{
	id: baseItem
	property alias text: text.text
	property alias subText: subText.text
	property alias subTextColor: subText.color

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
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
		font.pixelSize: Constants.normal_font_size
		anchors.top: text.bottom
		anchors.topMargin: Utils.dp(10)
		anchors.horizontalCenter: parent.horizontalCenter
		width: baseItem.width * 0.8
		wrapMode: Text.WordWrap
	}
}
