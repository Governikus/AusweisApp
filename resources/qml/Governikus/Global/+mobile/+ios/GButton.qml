/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import "Utils.js" as Utils

import Governikus.Style 1.0

/* Custom implementation to be replaced with template specialization of Qt.labs.controls Button*/
Rectangle {
	id: rect
	property alias text: textItem.text
	property var textStyle: enabled ? Style.text.button : Style.text.button_disabled
	property color buttonColor : Style.color.accent
	property int maxWidth: 0
	property int preferedWidth: Math.max(textItem.implicitWidth + (icon.visible ? (icon.width + icon.anchors.leftMargin) : 0) + (2 * 16), 88)
	property alias iconSource: icon.source
	property bool animationsDisabled: false

	signal clicked

	Accessible.role: Accessible.Button
	Accessible.name: text
	Accessible.onPressAction: clicked()

	color: enabled ? buttonColor : Style.color.accent_disabled
	height: Style.dimens.button_height
	width: maxWidth > 0 ? Math.min(maxWidth, preferedWidth) : preferedWidth
	clip: true
	radius: Style.dimens.button_radius

	Image {
		id: icon
		visible: source.toString().length > 0
		height: rect.height - 10
		width: height
		anchors.left: rect.left
		anchors.leftMargin: 5
		anchors.verticalCenter: rect.verticalCenter
	}

	GText {
		id: textItem

		anchors.left: rect.left
		anchors.right: rect.right
		anchors.verticalCenter: rect.verticalCenter
		horizontalAlignment: Text.AlignHCenter

		Accessible.ignored: true

		opacity: mouseArea.containsMouse ? 0.5 : 1
		anchors.leftMargin: icon.visible ? icon.width + icon.anchors.leftMargin : 0
		textStyle: rect.textStyle
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
