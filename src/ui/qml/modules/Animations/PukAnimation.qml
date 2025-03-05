/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import Governikus.Type

EnterPasswordAnimation {
	id: root

	source: "qrc:///animations/enter_puk.svg"
	symbolCount: 10
	symbolDistance: 0.07
	symbolSize: 0.113
	symbolX: 0.068
	symbolY: 0.61

	Image {
		source: "qrc:///images/npa.svg"
		sourceSize.height: parent.height * 0.085

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.534
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.125
		}
	}
	Rectangle {
		color: "#FF0000"
		height: parent.height * 0.14
		radius: parent.height * 0.016
		width: parent.width * 0.14

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.892
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.606
		}
		Rectangle {
			anchors.centerIn: parent
			color: UiPluginModel.highContrastEnabled ? "#FFFFFF" : root.tintColor
			height: parent.height / 2
			width: parent.width
		}
	}
}
