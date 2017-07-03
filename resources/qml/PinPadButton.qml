import QtQuick 2.5
import QtGraphicalEffects 1.0

import "global"

MouseArea {
	id: baseItem
	property alias text: textItem.text
	property alias source: imageItem.source

	Text {
		id: textItem
		anchors.centerIn: parent
		font.pixelSize: Utils.sp(24)
		wrapMode: Text.WordWrap
		color: "black"
	}

	Image {
		id: imageItem
		anchors.centerIn: parent
		height:Utils.dp(36)
		width: height
		fillMode: Image.PreserveAspectFit
	}

	Rectangle {
		id: darkLayer
		anchors.centerIn: parent
		height: width
		radius: width /2
		color: "#000000"
		opacity: 0.1
		SequentialAnimation on width {
			running: baseItem.pressed
			alwaysRunToEnd: true
			NumberAnimation { from: 0; to: Math.SQRT2 * baseItem.width }
			PauseAnimation { duration: 100 }
			PropertyAction { value: 0}
		}
	}
}
