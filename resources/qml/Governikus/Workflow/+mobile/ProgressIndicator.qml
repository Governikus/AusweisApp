/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0

Item {
	id: baseItem

	property alias imageIconSource: busyIcon.image
	property alias imagePhoneSource: phone.source

	TintableIcon {
		id: phone
		anchors.centerIn: parent
		desaturate: true
		height: parent.height * 0.5
		opacity: tintEnabled ? 0.7 : 1.0
		visible: baseItem.state === "off"
		width: height
	}
	Item {
		id: currentAction
		anchors.bottom: pCircle.top
		anchors.left: parent.left
		anchors.margins: Constants.component_spacing
		anchors.right: parent.right
		anchors.top: parent.top

		BusyImageIndicator {
			id: busyIcon
			anchors.centerIn: parent
			height: Math.min(parent.height - 40, 2 * pCircle.height)
			running: visible
			visible: baseItem.state === "one"
			width: height
		}
		CardReader {
			anchors.centerIn: parent
			cardAnimation: baseItem.state === "two"
			height: parent.height
			pinFieldAnimation: false
			visible: baseItem.state === "two"
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
