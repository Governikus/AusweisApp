import QtQuick 2.10

import "Utils.js" as Utils

MouseArea {
	id: baseItem

	property bool initialState: false
	property color color: Constants.blue
	readonly property alias isOn: toggleswitch.isOn
	signal switched()

	width: Utils.dp(60)
	height: Utils.dp(48)

	onClicked: toggleswitch.toggle()
	Component.onCompleted: toggleswitch.isOn = baseItem.initialState

	Item {
		id: toggleswitch
		anchors.fill: parent
		anchors.topMargin: Utils.dp(12)
		anchors.bottomMargin: Utils.dp(12)

		onIsOnChanged: updateState()
		property bool isOn: false
		readonly property int dragMax: width - height

		function updateState() {
			state = (isOn ? "on" : "off")
		}

		function toggle() {
			isOn = !isOn
			switched()
		}

		function captureSwitch() {
			state = "moving"
		}

		function releaseSwitch() {
			if (isOn !== (button.x > dragMax / 2)) toggle()
			updateState()
		}

		Rectangle {
			id: background
			anchors.fill: parent
			anchors.margins: parent.height / 4
			radius: height / 2
			color: isOn ? Qt.lighter(baseItem.color, 1.55) : "lightgray"
		}

		Rectangle {
			id: button
			height: parent.height
			width: height
			radius: width
			color: isOn ? baseItem.color : "darkgray"

			MouseArea {
				anchors.fill: parent
				drag.target: button
				drag.axis: Drag.XAxis
				drag.minimumX: 0
				drag.maximumX: toggleswitch.dragMax
				onClicked: toggleswitch.toggle()
				onPressed: toggleswitch.captureSwitch()
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
