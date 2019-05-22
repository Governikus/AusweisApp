import QtQuick 2.10
import QtGraphicalEffects 1.0

import "Utils.js" as Utils

/* Custom implementation to be replaced with template specialization of Qt.labs.controls Button*/
Rectangle {
	id: rect
	property alias text: textItem.text
	property color buttonColor : Constants.blue
	property int maxWidth: 0
	property int preferedWidth: Math.max(textItem.implicitWidth + (icon.visible ? (icon.width + icon.anchors.leftMargin) : 0) + (2 * Utils.dp(16)), Utils.dp(88))
	property alias iconSource: icon.source
	property bool animationsDisabled: false

	signal clicked

	color: enabled ? buttonColor : "#10000000"
	height: Constants.button_height
	width: maxWidth > 0 ? Math.min(maxWidth, preferedWidth) : preferedWidth
	clip: true

	Image {
		id: icon
		visible: source.toString().length > 0
		height: rect.height - Utils.dp(10)
		width: height
		anchors.left: rect.left
		anchors.leftMargin: Utils.dp(5)
		anchors.verticalCenter: rect.verticalCenter
	}

	Text {
		id: textItem
		anchors.left: rect.left
		anchors.right: rect.right
		anchors.verticalCenter: rect.verticalCenter
		color: enabled ? "white" : "#40000000"
		horizontalAlignment: Text.AlignHCenter
		opacity: mouseArea.containsMouse ? 0.5 : 1
		anchors.leftMargin: icon.visible ? icon.width + icon.anchors.leftMargin : 0
		font.pixelSize: Utils.dp(16)
		font.bold: true
	}

	MouseArea{
		id: mouseArea
		anchors.fill: parent
		preventStealing: true
		hoverEnabled: true
		onClicked: parent.clicked()
	}

	RadialGradient {
		x: mouseArea.mouseX - width * 0.5
		height: parent.height
		width: height * 2
		visible: !rect.animationsDisabled && mouseArea.pressed
		opacity: 1
		gradient: Gradient {
			GradientStop { position: 0.0; color: Qt.rgba(255,255,255,1) }
			GradientStop { position: 0.2; color: Qt.rgba(255,255,255,0.5) }
			GradientStop { position: 0.4; color: Qt.rgba(255,255,255,0) }
		}
	}
}
