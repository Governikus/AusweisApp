import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0

MouseArea {
	property alias imageSource: img.source
	property alias text: infoText.text
	property bool buttonActive

	height: img.height + infoText.height + 2 * img.anchors.topMargin + infoText.anchors.topMargin
	width: img.width + 2 * Utils.dp(10)

	Rectangle {
		height: 1
		width: parent.width * 1.5
		anchors.horizontalCenter: parent.horizontalCenter
		color: Constants.grey
	}
	Image {
		id: img
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		anchors.horizontalCenter: parent.horizontalCenter
		height: Utils.dp(50)
		fillMode: Image.PreserveAspectFit
		smooth: true
	}
	Text {
		id: infoText
		anchors.top: img.bottom
		anchors.topMargin: Utils.dp(10)
		anchors.horizontalCenter: img.horizontalCenter
		font.pixelSize: Constants.normal_font_size
		color: Constants.blue
	}
	Colorize {
		id: grayLevel
		source: img
		anchors.fill: img
		saturation: 0
		hue: 0
		lightness: 0.3
		cached: true
		visible: !parent.enabled || buttonActive
	}
	Colorize {
		id: grayLevel2
		source: infoText
		anchors.fill: infoText
		saturation: 0
		hue: 0
		lightness: 0.3
		cached: true
		visible: !parent.enabled || buttonActive
	}
}
