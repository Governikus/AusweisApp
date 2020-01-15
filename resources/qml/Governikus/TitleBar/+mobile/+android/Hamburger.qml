/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

import QtQuick 2.10


Item {
	id: baseItem

	implicitWidth: height
	visible: state !== "hidden"

	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Show navigation") + SettingsModel.translationTrigger

	Item {
		id: content
		anchors.centerIn: parent
		height: Math.min(parent.height, parent.width) / 2
		width: height

		readonly property double lineHeight: height / 8

		Rectangle {
			id: r0
			transformOrigin: Item.TopLeft
			x: 0
			y: content.height / 2 - 2.5 * content.lineHeight
			width: content.width
			height: content.lineHeight
			antialiasing: true
			color: Constants.white
		}

		Rectangle {
			id: r1
			transformOrigin: Item.TopLeft
			x: 0
			y: content.height / 2 - 0.5 * content.lineHeight
			width: content.width
			height: content.lineHeight
			antialiasing: true
			color: Constants.white
		}

		Rectangle {
			id: r2
			transformOrigin: Item.TopLeft
			x: 0
			y: content.height / 2 + 1.5 * content.lineHeight
			width: content.width
			height: content.lineHeight
			antialiasing: true
			color: Constants.white
		}
	}

	states: [
		State {
			id: backState
			name: "back"

			readonly property double itemArrowWidth: 0.7 * content.width
			readonly property double delta0: Math.sqrt(Math.pow(itemArrowWidth, 2) / 2)
			readonly property double delta1: Math.sqrt(Math.pow(content.lineHeight, 2) / 2)

			PropertyChanges { target: content; rotation: 180 }

			PropertyChanges {
				target: r0
				rotation: 45
				x: content.width - backState.delta0
				y: content.height / 2 - backState.delta0
				width: backState.itemArrowWidth
			}
			PropertyChanges {
				target: r1
				rotation: 0
				x: 0
				y: (content.height - content.lineHeight) / 2
				width: content.width - content.lineHeight
			}
			PropertyChanges {
				target: r2
				rotation: -45
				x: content.width - backState.delta0 - backState.delta1
				y: content.height / 2 + backState.delta0 - backState.delta1
				width: backState.itemArrowWidth
			}

			PropertyChanges {
				target: baseItem
				Accessible.name: qsTr("Back") + SettingsModel.translationTrigger
			}
		},

		State {
			id: cancelState
			name: "cancel"

			readonly property double delta0: Math.sqrt(Math.pow(content.lineHeight, 2) / 2)
			readonly property double delta1: Math.sqrt(2 * Math.pow(content.width, 2)) - content.lineHeight

			PropertyChanges { target: content; rotation: 180 }

			PropertyChanges {
				target: r0
				rotation: 45
				x: cancelState.delta0
				y: 0
				width: cancelState.delta1
			}
			PropertyChanges {
				target: r1
				rotation: 0
				x: content.width / 2
				y: (content.height - content.lineHeight) / 2
				width: 0
			}
			PropertyChanges {
				target: r2
				rotation: -45
				x: 0
				y: content.height - cancelState.delta0
				width: cancelState.delta1
			}

			PropertyChanges {
				target: baseItem
				Accessible.name: qsTr("Cancel") + SettingsModel.translationTrigger
			}
		}
	]

	transitions: [
		Transition { from: "hidden" },
		Transition {
			RotationAnimation { target: content; direction: RotationAnimation.Clockwise; duration: 300; easing.type: Easing.InOutQuad }
			PropertyAnimation { targets: [r0, r1, r2]; properties: "rotation, x, y, width"; duration: 300; easing.type: Easing.InOutQuad }
		}
	]
}
