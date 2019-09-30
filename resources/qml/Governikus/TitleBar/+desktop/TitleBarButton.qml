/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.NotificationModel 1.0


Button {
	id: button

	property alias source: image.source
	function notify() {
		blinkerAnimation.start()
	}

	width: height

	Accessible.role: Accessible.Button

	enabled: visible
	background: Image {
		id: image

		height: button.height * (button.down ? 0.9 : 1)
		width: height

		anchors.centerIn: parent

		sourceSize.height: height
		sourceSize.width: width

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

		text: parent.Accessible.name
		delay: 500
	}
}
