/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.NotificationModel 1.0

Button {
	id: button

	property color iconColor: Style.text.header_inverse.textColor
	property alias source: image.source

	function notify() {
		blinkerAnimation.start();
	}

	enabled: visible
	padding: 0
	width: height

	background: Item {
	}
	contentItem: Item {
		anchors.centerIn: parent
		height: button.height
		width: height

		TintableIcon {
			id: image

			readonly property color pressColor: Qt.darker(iconColor, Constants.highlightDarkerFactor)

			anchors.fill: parent
			sourceSize.height: height
			sourceSize.width: width
			tintColor: button.pressed ? pressColor : iconColor
		}
		Rectangle {
			id: blinker
			anchors.fill: parent
			anchors.margins: image.height / -4
			color: NotificationModel.lastType === "developermode" ? Constants.red : Constants.green
			opacity: 0
			radius: height / 4

			SequentialAnimation {
				id: blinkerAnimation
				loops: 3

				PropertyAnimation {
					duration: 300
					from: 0
					property: "opacity"
					target: blinker
					to: 0.5
				}
				PropertyAnimation {
					duration: 300
					from: 0.5
					property: "opacity"
					target: blinker
					to: 0
				}
			}
		}
	}

	FocusFrame {
		isOnLightBackground: false
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor
		hoverEnabled: true

		onPressed: mouse => {
			mouse.accepted = false;
		}
	}
	ToolTip {
		delay: 500
		text: button.text
		visible: mouseArea.containsMouse
	}
}
