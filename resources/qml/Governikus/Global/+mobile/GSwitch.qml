/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import "Utils.js" as Utils

import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

MouseArea {
	id: baseItem

	property bool initialState: false
	property color color: Style.color.accent
	readonly property alias isOn: toggleswitch.isOn
	signal switched()

	width: 60
	height: 48

	Accessible.role: Accessible.CheckBox
	Accessible.name: qsTr("Switch") + SettingsModel.translationTrigger
	Accessible.checkable: true
	Accessible.checked: isOn
	Accessible.onPressAction: if (Qt.platform.os === "ios") clicked(null)

	onClicked: toggleswitch.toggle()
	Component.onCompleted: toggleswitch.isOn = baseItem.initialState

	Item {
		id: toggleswitch
		anchors.fill: parent
		anchors.topMargin: 12
		anchors.bottomMargin: 12

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
			color: isOn ? Qt.lighter(baseItem.color, 1.4) : Qt.darker(Style.color.accent_disabled, 1.1)
		}

		Rectangle {
			id: button
			height: parent.height
			width: height
			radius: width
			color: isOn ? baseItem.color : Style.color.accent_disabled

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
