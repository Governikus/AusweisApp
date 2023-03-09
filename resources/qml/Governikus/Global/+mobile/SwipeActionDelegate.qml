/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0

Item {
	id: baseItem

	property string actionAccessibleName
	property alias actionColor: actionBackground.color
	property alias actionIcon: actionImage.source
	property alias actionIconTintColor: actionImage.tintColor
	property alias contentItem: content.children
	property alias pressed: mouseArea.downOnContent

	signal actionClicked
	signal clicked

	height: content.childrenRect.height
	width: content.childrenRect.width

	Rectangle {
		id: actionBackground
		width: Math.abs(mouseArea.contentX)

		anchors {
			bottom: parent.bottom
			right: parent.right
			top: parent.top
		}
	}
	TintableIcon {
		id: actionImage
		Accessible.name: actionAccessibleName
		Accessible.role: Accessible.Button
		width: Math.abs(mouseArea.actionOpenOffset) - anchors.margins * 2

		Accessible.onPressAction: baseItem.actionClicked()

		anchors {
			bottom: parent.bottom
			margins: Constants.component_spacing
			right: parent.right
			top: parent.top
		}
	}
	Item {
		id: content
		height: parent.height
		width: parent.width

		Behavior on x  {
			NumberAnimation {
				duration: Constants.animation_duration

				onRunningChanged: {
					if (!running && content.x <= -content.width) {
						baseItem.actionClicked();
					}
				}
			}
		}
	}
	MouseArea {
		id: mouseArea

		readonly property real actionOpenOffset: -parent.width / 4
		property real contentStartX: 0.0
		property alias contentX: content.x
		readonly property bool deleteOffsetReached: contentX < 2 * actionOpenOffset
		property bool didDrag: false
		readonly property bool downOnAction: pressed && !downOnContent
		readonly property bool downOnContent: pressed && startX < contentStartX + content.width
		readonly property bool isActionOpen: contentX === actionOpenOffset
		property real startX: 0.0

		anchors.fill: parent

		onPressed: mouse => {
			startX = mouse.x;
			contentStartX = content.x;
			didDrag = false;
		}
		onReleased: {
			if (didDrag) {
				if (contentX > actionOpenOffset / 2) {
					contentX = 0;
				} else if (deleteOffsetReached) {
					contentX = drag.minimumX;
				} else {
					contentX = actionOpenOffset;
				}
			} else if (downOnContent) {
				if (isActionOpen) {
					contentX = 0;
				} else {
					baseItem.clicked();
				}
			} else if (downOnAction) {
				baseItem.actionClicked();
			}
		}

		drag {
			axis: Drag.XAxis
			filterChildren: true
			maximumX: 0
			minimumX: -content.width
			target: content
			threshold: 10

			onActiveChanged: didDrag = true
		}
	}
}
