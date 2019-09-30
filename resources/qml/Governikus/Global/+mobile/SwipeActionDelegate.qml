/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

Rectangle {
	id: baseItem

	property alias contentItem: content.children
	property alias pressed: mouseArea.downOnContent
	property alias actionColor: baseItem.color
	property alias actionIcon: actionImage.source
	property string actionAccessibleName

	signal clicked
	signal actionClicked

	height: content.childrenRect.height
	width: parent.width

	Image {
		Accessible.role: Accessible.Button
		Accessible.name: actionAccessibleName
		Accessible.onPressAction: baseItem.actionClicked()

		id: actionImage
		anchors {
			top: parent.top
			bottom: parent.bottom
			right: parent.right
			margins: 20
		}
		width: Math.abs(mouseArea.actionOpenOffset) - anchors.margins * 2
		fillMode: Image.PreserveAspectFit
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
