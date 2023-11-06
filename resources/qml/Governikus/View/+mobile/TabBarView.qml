/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Type.ApplicationModel

StackView {
	id: root

	readonly property bool animationEnabled: !ApplicationModel.isScreenReaderRunning()

	function doActivate() {
		if (visible && currentItem && typeof currentItem.activate === "function") {
			currentItem.activate();
		}
	}

	popEnter: enterAnimation.createObject(root)
	popExit: exitAnimation.createObject(root)
	pushEnter: enterAnimation.createObject(root, {
			"reversed": true
		})
	pushExit: exitAnimation.createObject(root)
	replaceEnter: enterAnimation.createObject(root, {
			"reversed": true
		})
	replaceExit: exitAnimation.createObject(root)

	onCurrentItemChanged: doActivate()
	onVisibleChanged: doActivate()

	Component {
		id: enterAnimation

		Transition {
			readonly property bool reversed: false

			enabled: animationEnabled

			ParallelAnimation {
				NumberAnimation {
					duration: Constants.animation_duration
					easing.type: Easing.InQuint
					from: 0
					property: "opacity"
					to: 1
				}
				NumberAnimation {
					duration: Constants.animation_duration * 2
					easing.type: Easing.OutCubic
					from: (root.mirrored ? -0.3 : 0.3) * root.width * (reversed ? 1 : -1)
					property: "x"
					to: 0
				}
			}
		}
	}
	Component {
		id: exitAnimation

		Transition {
			enabled: animationEnabled

			NumberAnimation {
				duration: Constants.animation_duration
				easing.type: Easing.OutQuint
				from: 1
				property: "opacity"
				to: 0
			}
		}
	}
}
