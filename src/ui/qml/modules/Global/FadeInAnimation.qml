/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global

Item {
	id: root

	required property QtObject target

	states: [
		State {
			name: "visible"
			when: root.target.visible

			PropertyChanges {
				opacity: 1.0
				target: root.target
			}
		},
		State {
			name: "invisible"
			when: !root.target.visible

			PropertyChanges {
				opacity: 0.0
				target: root.target
			}
		}
	]
	transitions: [
		Transition {
			from: "invisible"
			to: "visible"

			PropertyAnimation {
				duration: Constants.animation_duration * 4
				easing.type: Easing.OutCubic
				property: "opacity"
				target: root.target
				to: 1
			}
		}
	]
}
