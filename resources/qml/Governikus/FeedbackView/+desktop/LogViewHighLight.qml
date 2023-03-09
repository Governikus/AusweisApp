/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQml 2.15
import QtQuick 2.15
import Governikus.Style 1.0

Item {
	property var currentItem: null
	property int textSize: Style.text.hint.textSize

	visible: currentItem ? currentItem.activeFocus : false

	Rectangle {
		color: plugin.showFocusIndicator && currentItem ? currentItem.color : Style.color.transparent
		height: width / 4
		width: textSize / 1.6

		anchors {
			left: parent.left
			top: parent.top
			topMargin: textSize
		}
		Timer {
			interval: 500
			repeat: true
			running: visible

			onTriggered: parent.opacity = parent.opacity ? 0 : 1
		}
	}
}
