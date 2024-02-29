/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

Item {
	property alias animated: animation.animated

	implicitHeight: animation.implicitHeight
	implicitWidth: animation.implicitWidth

	EnterPasswordAnimation {
		id: animation

		source: "qrc:///animations/enter_puk.svg"
		starCount: 10
		starDistance: 0.07
		starSize: 0.113
		starX: 0.06
		starY: 0.66
	}
	Image {
		source: "qrc:///images/npa.svg"
		sourceSize.height: parent.height * 0.069

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.534
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.1005
		}
	}
	Rectangle {
		color: "#FF0000"
		height: parent.height * 0.115
		radius: parent.height * 0.016
		width: parent.width * 0.14

		anchors {
			horizontalCenter: parent.left
			horizontalCenterOffset: parent.width * 0.892
			verticalCenter: parent.top
			verticalCenterOffset: parent.height * 0.661
		}
		Rectangle {
			anchors.centerIn: parent
			color: plugin.highContrastEnabled ? "#FFFFFF" : animation.tintColor
			height: parent.height / 2
			width: parent.width
		}
	}
}
