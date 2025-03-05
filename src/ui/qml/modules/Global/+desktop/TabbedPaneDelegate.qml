/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Style
import Governikus.View

Item {
	id: root

	//: LABEL DESKTOP %1 is the current selected Page of %2 Pages
	readonly property string a11yPageIndicator: qsTr("%1 of %2").arg(index + 1).arg(ListView.view.count)
	property int highlightedIndex: -1
	required property int index
	readonly property bool isFirstItem: index === 0
	readonly property bool isLastItem: index === ListView.view.count - 1
	required property var model
	required property string modelData
	readonly property bool nextItemIsHighlighted: index === ListView.view.currentIndex - 1 || index === root.highlightedIndex - 1

	Accessible.description: Qt.platform.os === "windows" ? a11yPageIndicator : ""
	Accessible.focusable: true
	Accessible.name: {
		if (Qt.platform.os === "windows") {
			return sectionName.text;
		}
		//: LABEL DESKTOP
		return sectionName.text + ", " + qsTr("Tab selected") + ", " + a11yPageIndicator;
	}
	Accessible.role: Accessible.PageTab
	height: sectionName.height + 2 * Style.dimens.pane_padding
	width: ListView.view.width

	StatefulColors {
		id: colors

		checkedCondition: root.ListView.isCurrentItem
		disabledCondition: false
		groupMember: true
		hoveredCondition: mouseArea.containsMouse
		pressedCondition: mouseArea.pressed
		statefulControl: root
	}
	RoundedRectangle {
		anchors.fill: parent
		borderColor: colors.paneBorder
		bottomLeftCorner: root.isLastItem
		bottomRightCorner: root.isLastItem
		color: colors.paneBackground
		radius: Style.dimens.pane_radius
		topLeftCorner: root.isFirstItem
		topRightCorner: root.isFirstItem
	}
	GText {
		id: sectionName

		activeFocusOnTab: false
		color: colors.textNormal
		elide: Text.ElideRight
		maximumLineCount: 2
		text: root.modelData
		textStyle: Style.text.subline

		anchors {
			left: parent.left
			leftMargin: Style.dimens.pane_padding
			right: parent.right
			rightMargin: Style.dimens.pane_padding
			verticalCenter: parent.verticalCenter
		}
	}
	GSeparator {
		id: horizontalSeparator

		visible: !root.isLastItem && !root.nextItemIsHighlighted && GraphicsInfo.api !== GraphicsInfo.Software

		states: [
			State {
				name: "active"
				when: root.ListView.isCurrentItem

				PropertyChanges {
					horizontalSeparator.visible: false
				}
			},
			State {
				name: "pressed"
				when: mouseArea.pressed

				PropertyChanges {
					horizontalSeparator.visible: false
				}
			},
			State {
				name: "hovered"
				when: mouseArea.containsMouse

				PropertyChanges {
					horizontalSeparator.visible: false
				}
			}
		]

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: Style.dimens.pane_padding
			right: parent.right
			rightMargin: Style.dimens.pane_padding
		}
	}
	FocusFrame {
		borderColor: sectionName.color
		framee: sectionName
	}
	MouseArea {
		id: mouseArea

		function updateHighlight() {
			if (containsMouse || pressed) {
				root.highlightedIndex = root.index;
			} else {
				if (root.highlightedIndex === root.index) {
					root.highlightedIndex = -1;
				}
			}
		}

		anchors.fill: parent
		hoverEnabled: true

		onClicked: {
			root.ListView.view.itemAtIndex(root.index).forceActiveFocus(Qt.MouseFocusReason);
			root.ListView.view.currentIndex = root.index;
		}
		onContainsMouseChanged: updateHighlight()
		onPressedChanged: updateHighlight()
	}
}
