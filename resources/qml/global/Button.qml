import QtQuick 2.5
import QtGraphicalEffects 1.0

import "."

/* Custom implementation to be replaced with template specialization of Qt.labs.controls Button*/
Rectangle {
	property alias text: textItem.text
	property color buttonColor : Constants.blue

	signal clicked

	color: enabled ? buttonColor : "#10000000"
	height: Constants.button_height
	width: parent.width - 2 * Utils.dp(20)
	clip: true

	Text {
		id: textItem
		anchors.centerIn: parent
		color: enabled ? "white" : "#40000000"
		opacity: mouseArea.containsMouse ? 0.5 : 1
		font.pixelSize: Utils.dp(16)
	}
	MouseArea{
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		onClicked: parent.clicked()
	}

	RadialGradient {
		x: mouseArea.mouseX - width * 0.5
		height: parent.height
		width: height * 2
		visible: mouseArea.pressed
		opacity: 1
		gradient: Gradient {
			GradientStop { position: 0.0; color: Qt.rgba(255,255,255,1) }
			GradientStop { position: 0.2; color: Qt.rgba(255,255,255,0.5) }
			GradientStop { position: 0.4; color: Qt.rgba(255,255,255,0) }
		}
	}
}
