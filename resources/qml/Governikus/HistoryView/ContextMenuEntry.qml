import QtQuick 2.5
import QtGraphicalEffects 1.0

import Governikus.Global 1.0

MouseArea {
	id: mouseArea
	property alias text: textItem.text
	property color buttonColor: Constants.white
	readonly property int touchPadding: Utils.dp(3);

	height: textItem.height + 2 * touchPadding
	width: Math.max(textItem.implicitWidth + 2 * touchPadding, parent.width)

	state: "normal"
	states: [
		State { name: "normal"; when: !mouseArea.pressed
			PropertyChanges { target: darkLayer; width: 0 }
		},
		State { name: "pressed"; when: mouseArea.pressed
			PropertyChanges { target: darkLayer; width: 2 * rect.width }
		}
	]
	transitions: [
		Transition {
			from: "normal"; to: "pressed"; reversible: false
			PropertyAnimation { target: darkLayer; property: "width"}
		}
	]

	Rectangle {
		id: rect
		anchors.fill: parent
		color: buttonColor

		Item {
			anchors.fill: parent
			clip: true
			Rectangle {
				id: darkLayer
				x: mouseArea.containsMouse ? mouseArea.mouseX - width * 0.5 : 0
				height: parent.height
				color: Constants.black
				opacity: 0.2
			}
		}
	}

	Text {
		id: textItem
		color: Constants.secondary_text
		anchors.left: rect.left
		anchors.leftMargin: touchPadding
		anchors.verticalCenter: rect.verticalCenter
		font.pixelSize: Constants.titlebar_font_size
	}
}

