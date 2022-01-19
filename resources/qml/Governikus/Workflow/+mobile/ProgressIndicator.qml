/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtGraphicalEffects 1.12

import Governikus.Global 1.0


Item {
	id: baseItem
	property alias imageIconSource: busyIcon.image
	property alias imagePhoneSource: phone.source

	Image {
		id: phone
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
			id: busyIcon
			anchors.centerIn: parent
			width: height
			height: Math.min(parent.height - 40, 2 * pCircle.height)
			running: visible
			visible: baseItem.state === "one"
		}

		CardReader {
			anchors.centerIn: parent
			height: parent.height
			visible: baseItem.state === "two"
			cardAnimation: baseItem.state === "two"
			pinFieldAnimation: false
		}
	}

	ProgressCircle {
		id: pCircle
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		state: baseItem.state
		visible: baseItem.state === "one" || baseItem.state === "two"
	}
}
