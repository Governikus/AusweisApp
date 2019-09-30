/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Style 1.0

import "Utils.js" as Utils

import Governikus.Style 1.0

/*
 * Custom implementation to be replaced with template specialization of Qt.labs.controls Button
 * Android style guide for material design is adapted.
 */
Item {
	id: baseItem
	property alias text: textItem.text
	property var textStyle: enabled ? Style.text.button : Style.text.button_disabled
	property color buttonColor: Style.color.accent
	property int maxWidth: 0
	property alias iconSource: icon.source
	property bool animationsDisabled: false

	signal clicked

	Accessible.role: Accessible.Button
	Accessible.name: text

	height: Style.dimens.button_height
	width: Math.max(textItem.implicitWidth + (icon.visible ? (icon.width + icon.anchors.leftMargin) : 0) + 2 * 16, 88)

	state: "normal"
	states: [
		State { name: "normal"; when: baseItem.animationsDisabled || !mouseArea.pressed
			PropertyChanges { target: darkLayer; width: 0 }
			PropertyChanges { target: shadow; verticalOffset: 2 }
		},
		State { name: "pressed"; when: !baseItem.animationsDisabled && mouseArea.pressed
			PropertyChanges { target: darkLayer; width: 2 * rect.width }
			PropertyChanges { target: shadow; verticalOffset: 8 }
		}
	]
	transitions: [
		Transition {
			from: "normal"; to: "pressed"; reversible: false
			PropertyAnimation { target: darkLayer; property: "width"}
			PropertyAnimation { target: shadow; property: "verticalOffset"}
		}
	]

	Rectangle {
		id: rect
		anchors.fill: parent
		color: enabled ? buttonColor : Style.color.accent_disabled
		radius: Style.dimens.button_radius

		Item {
			anchors.fill: parent
			clip: true
			Rectangle {
				id: darkLayer
				x: mouseArea.containsMouse ? mouseArea.mouseX - width * 0.5 : 0
				height: parent.height
				color: Constants.black
				opacity: 0.2
				radius: Style.dimens.button_radius
			}
		}
	}

	DropShadow {
		id: shadow
		anchors.fill: rect
		radius: 8.0
		fast: true
		color: "#40000000"
		source: rect
	}

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
		anchors.leftMargin: icon.visible ? icon.width + icon.anchors.leftMargin : 0

		Accessible.ignored: true

		horizontalAlignment: Text.AlignHCenter
		font.capitalization: Font.AllUppercase
		textStyle: baseItem.textStyle
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		preventStealing: true
		hoverEnabled: true
		onClicked: parent.clicked()
	}
}

