/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.NotificationModel 1.0


Button {
	id: button

	property alias source: image.source
	property color iconColor: Style.text.header_inverse.textColor
	function notify() {
		blinkerAnimation.start()
	}

	width: height

	enabled: visible

	padding: 0
	background: Item {}
	contentItem: Item {

		height: button.height
		width: height

		anchors.centerIn: parent

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

			opacity: 0
			radius: height / 4
			color: NotificationModel.lastType === "developermode" ? Constants.red : Constants.green

			SequentialAnimation {
				id: blinkerAnimation

				loops: 3

				PropertyAnimation { target: blinker; property: "opacity"; from: 0; to: 0.5; duration: 300 }
				PropertyAnimation { target: blinker; property: "opacity"; from: 0.5; to: 0; duration: 300 }
			}
		}
	}

	FocusFrame {}

	MouseArea {
		id: mouseArea

		anchors.fill: parent
		cursorShape: Qt.PointingHandCursor
		onPressed: mouse.accepted = false
		hoverEnabled: true
	}

	ToolTip {
		visible: mouseArea.containsMouse

		text: button.text
		delay: 500
	}
}
