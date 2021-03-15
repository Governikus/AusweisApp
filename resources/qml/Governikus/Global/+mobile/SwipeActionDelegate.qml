/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0

Rectangle {
	id: baseItem

	property alias contentItem: content.children
	property alias pressed: mouseArea.downOnContent
	property alias actionColor: baseItem.color
	property alias actionIcon: actionImage.source
	property alias actionIconTintColor: actionImage.tintColor
	property string actionAccessibleName

	signal clicked
	signal actionClicked

	height: content.childrenRect.height
	width: content.childrenRect.width

	TintableIcon {
		id: actionImage

		Accessible.role: Accessible.Button
		Accessible.name: actionAccessibleName
		Accessible.onPressAction: baseItem.actionClicked()

		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
			margins: Constants.component_spacing
		}
		width: Math.abs(mouseArea.actionOpenOffset) - anchors.margins * 2
	}

	Item {
		id: content

		width: parent.width
		height: parent.height

		Behavior on x {
			NumberAnimation {
				duration: Constants.animation_duration
				onRunningChanged: {
					if (!running && content.x <= -content.width) {
						baseItem.actionClicked()
					}
				}
			}
		}
	}

	MouseArea {
		id: mouseArea

		readonly property real actionOpenOffset: -parent.width / 4
		readonly property bool isActionOpen: contentX === actionOpenOffset

		readonly property bool downOnContent: pressed && startX < contentStartX + content.width
		readonly property bool downOnAction: pressed && !downOnContent
		readonly property bool deleteOffsetReached: contentX < 2 * actionOpenOffset
		property bool didDrag: false

		property real startX: 0.0
		property alias contentX: content.x
		property real contentStartX: 0.0

		anchors.fill: parent

		drag {
			target: content
			axis: Drag.XAxis
			maximumX: 0
			minimumX: -content.width
			filterChildren: true
			threshold: 10
			onActiveChanged: didDrag = true
		}

		onPressed: {
			startX = mouse.x
			contentStartX = content.x
			didDrag = false
		}

		onReleased: {
			if (didDrag) {
				if (contentX > actionOpenOffset / 2) {
					contentX = 0
				} else if (deleteOffsetReached) {
					contentX = drag.minimumX
				} else {
					contentX = actionOpenOffset
				}
			} else if (downOnContent) {
				if (isActionOpen) {
					contentX = 0
				} else {
					baseItem.clicked()
				}
			} else if (downOnAction) {
				baseItem.actionClicked()
			}
		}
	}
}
