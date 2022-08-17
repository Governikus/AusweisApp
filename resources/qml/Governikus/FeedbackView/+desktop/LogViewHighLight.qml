/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQml 2.15
import QtQuick 2.15

import Governikus.Style 1.0


Item {
	property int textSize: Style.text.hint.textSize
	property var currentItem: null

	visible: currentItem ? currentItem.activeFocus : false

	Rectangle {
		anchors {
			left: parent.left
			top: parent.top
			topMargin: textSize
		}

		width: textSize / 1.6
		height: width / 4

		color: plugin.showFocusIndicator && currentItem ? currentItem.color : Style.color.transparent

		Timer {
			interval: 500
			running: visible
			repeat: true
			onTriggered: parent.opacity = parent.opacity ? 0 : 1
		}
	}
}
