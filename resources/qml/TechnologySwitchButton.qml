import QtQuick 2.5
import QtGraphicalEffects 1.0

import "global"

MouseArea {
	property alias imageSource: img.source
	property alias text: infoText.text

	height: img.height + 2 * img.anchors.topMargin
	width: img.width + infoText.anchors.leftMargin + infoText.width

	Rectangle {
		height: 1
		width: parent.width * 1.2
		anchors.horizontalCenter: parent.horizontalCenter
		color: Constants.grey
	}
	Image {
		id: img
		sourceSize.height: 604
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(20)
		height: Utils.dp(50)
		fillMode: Image.PreserveAspectFit
		smooth: true
	}
	Text {
		id: infoText
		anchors.left: img.right
		anchors.leftMargin: Utils.dp(10)
		anchors.verticalCenter: img.verticalCenter
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
		visible: !parent.enabled
	}
	Colorize {
		id: grayLevel2
		source: infoText
		anchors.fill: infoText
		saturation: 0
		hue: 0
		lightness: 0.3
		cached: true
		visible: !parent.enabled
	}
}
