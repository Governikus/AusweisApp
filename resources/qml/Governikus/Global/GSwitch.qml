import QtQuick 2.7

import "Utils.js" as Utils
import "." as Gov

MouseArea {
	property alias isOn: toggleswitch.isOn
	width: Utils.dp(60)
	height: Utils.dp(48)

	onClicked: toggleswitch.toggle()

	Item {
		id: toggleswitch
		anchors.fill: parent
		anchors.topMargin: Utils.dp(12)
		anchors.bottomMargin: Utils.dp(12)

		property bool isOn: false
		onIsOnChanged: state = (isOn ? "on" : "off")

		readonly property int dragMax: width - height

		function toggle() {
			isOn = !isOn
		}

		function releaseSwitch() {
			isOn = (button.x > dragMax / 2)
			button.x = (isOn ? dragMax : 0)
		}

		Rectangle {
			id: background
			anchors.fill: parent
			anchors.margins: parent.height / 4
			radius: height / 2
			color: isOn ? Qt.lighter(Gov.Constants.blue, 1.55) : "lightgray"
		}

		Rectangle {
			id: button
			height: parent.height
			width: height
			radius: width
			color: isOn ? Gov.Constants.blue : "darkgray"

			MouseArea {
				anchors.fill: parent
				drag.target: button
				drag.axis: Drag.XAxis
				drag.minimumX: 0
				drag.maximumX: toggleswitch.dragMax
				onClicked: toggleswitch.toggle()
				onReleased: toggleswitch.releaseSwitch()
			}
		}

		states: [
			State {
				name: "on"
				PropertyChanges { target: button; x: toggleswitch.dragMax }
			},
			State {
				name: "off"
				PropertyChanges { target: button; x: 0 }
			}
		]

		transitions: Transition {
			NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad; duration: 200 }
		}
	}
}
