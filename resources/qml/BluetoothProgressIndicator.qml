import QtQuick 2.5
import QtGraphicalEffects 1.0

import "global"


Item {
	id: baseItem

	Image {
		id: phone
		source: "qrc:///images/phone_bluetooth.svg"
		height: parent.height * 0.5
		width: height
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		visible: baseItem.state === "off"
	}
	Colorize {
		id: grayLevel
		source: phone
		anchors.fill: phone
		saturation: 0
		hue: 0
		lightness: 0.3
		cached: true
		visible: baseItem.state === "off"
	}

	Item {
		id: currentAction
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: pCircle.top
		anchors.margins: Constants.component_spacing

		BusyImageIndicator {
			anchors.centerIn: parent
			width: height
			height: parent.height - Utils.dp(40)
			image: "qrc:///images/icon_Bluetooth.svg"
			running: visible
			visible: baseItem.state === "one"
		}

		CardReader {
			anchors.centerIn: parent
			height: parent.height
			visible: baseItem.state === "two" || baseItem.state === "three"
			cardAnimation: baseItem.state === "two"
			pinFieldAnimation: baseItem.state === "three"
		}
	}

	ProgressCircle {
		id: pCircle
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		state: baseItem.state
		visible: baseItem.state === "one" || baseItem.state === "two" || baseItem.state === "three"
	}
}
