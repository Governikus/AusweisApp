/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.Type

EnterPasswordAnimation {
	id: animation

	source: "qrc:///animations/enter_transport_pin.svg"
	symbolCount: 5
	symbolDistance: 0.113
	symbolSize: 0.114
	symbolX: 0.435
	symbolY: 0.6

	Image {
		source: "qrc:///images/npa.svg"
		sourceSize.height: parent.height * 0.082

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.6144
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.1243
		}
	}
	Rectangle {
		color: "#FF0000"
		height: parent.height * 0.14
		radius: parent.height * 0.02
		width: parent.width * 0.18

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.139
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.604
		}
		Rectangle {
			anchors.centerIn: parent
			color: UiPluginModel.highContrastEnabled ? "#FFFFFF" : animation.tintColor
			height: parent.height / 2
			width: parent.width
		}
	}
}
