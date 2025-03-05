/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

import Governikus.Type
import Governikus.Style

StackView {
	id: root

	readonly property bool animationEnabled: !ApplicationModel.isScreenReaderRunning && SettingsModel.useAnimations

	function doActivate() {
		if (visible && currentItem && (currentItem as BaseController)) {
			(currentItem as BaseController).activate();
		}
	}

	popEnter: EnterAnimation {
	}
	popExit: ExitAnimation {
	}
	pushEnter: EnterAnimation {
		reversed: true
	}
	pushExit: ExitAnimation {
	}
	replaceEnter: EnterAnimation {
		reversed: true
	}
	replaceExit: ExitAnimation {
	}

	onCurrentItemChanged: doActivate()
	onVisibleChanged: doActivate()

	component EnterAnimation: Transition {
		id: transition

		property bool reversed: false

		enabled: root.animationEnabled

		ParallelAnimation {
			NumberAnimation {
				duration: Style.animation_duration
				easing.type: Easing.InQuint
				from: 0
				property: "opacity"
				to: 1
			}
			NumberAnimation {
				duration: Style.animation_duration * 2
				easing.type: Easing.OutCubic
				from: (root.mirrored ? -0.3 : 0.3) * root.width * (transition.reversed ? 1 : -1)
				property: "x"
				to: 0
			}
		}
	}
	component ExitAnimation: Transition {
		enabled: root.animationEnabled

		NumberAnimation {
			duration: Style.animation_duration
			easing.type: Easing.OutQuint
			from: 1
			property: "opacity"
			to: 0
		}
	}
}
