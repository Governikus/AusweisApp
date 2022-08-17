/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Type.ApplicationModel 1.0

StackView {
	id: root

	function doActivate() {
		if (visible && currentItem && typeof currentItem.activated === "function") {
			currentItem.activated()
		}
	}

	readonly property bool animationEnabled: !ApplicationModel.isScreenReaderRunning()

	onVisibleChanged: doActivate()
	onCurrentItemChanged: doActivate()

	Component {
		id: enterAnimation

		Transition {
			readonly property bool reversed: false

			enabled: animationEnabled

			ParallelAnimation {
				NumberAnimation {
					property: "opacity"
					from: 0
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.InQuint
				}
				NumberAnimation {
					property: "x"
					from: (root.mirrored ? -0.3 : 0.3) * root.width * (reversed ? 1 : -1)
					to: 0
					duration: Constants.animation_duration * 2
					easing.type: Easing.OutCubic
				}
			}
		}
    }

	Component {
		id: exitAnimation

		Transition {
			enabled: animationEnabled

			NumberAnimation {
				property: "opacity"
				from: 1
				to: 0
				duration: Constants.animation_duration
				easing.type: Easing.OutQuint
			}
		}
    }

    popEnter: enterAnimation.createObject(root)
    popExit: exitAnimation.createObject(root)

    pushEnter: enterAnimation.createObject(root, {reversed: true})
    pushExit: exitAnimation.createObject(root)

    replaceEnter: enterAnimation.createObject(root, {reversed: true})
    replaceExit: exitAnimation.createObject(root)
}
