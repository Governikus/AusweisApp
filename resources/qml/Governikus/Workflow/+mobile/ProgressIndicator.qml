/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0


Item {
	id: baseItem
	property alias imageIconSource: busyIcon.image
	property alias imagePhoneSource: phone.source

	TintableIcon {
		id: phone

		visible: baseItem.state === "off"

		height: parent.height * 0.5
		width: height
		anchors.centerIn: parent

		desaturate: true
		opacity: tintEnabled ? 0.7 : 1.0
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
