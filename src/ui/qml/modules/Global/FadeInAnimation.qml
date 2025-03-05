/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Style

Item {
	id: root

	required property Item target

	states: [
		State {
			name: "visible"
			when: root.target.visible

			PropertyChanges {
				root.target.opacity: 1.0
			}
		},
		State {
			name: "invisible"
			when: !root.target.visible

			PropertyChanges {
				root.target.opacity: 0.0
			}
		}
	]
	transitions: [
		Transition {
			from: "invisible"
			to: "visible"

			PropertyAnimation {
				duration: Style.animation_duration * 4
				easing.type: Easing.OutCubic
				property: "opacity"
				target: root.target
				to: 1
			}
		}
	]
}
