/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: titleBar

	enum TitleAlignment {
		Center = 0,
		Left
	}

	property alias titleBarOpacity: background.opacity

	property alias title: titleText.text
	property var leftAction
	property var rightAction
	property var subTitleBarAction

	property var color
	property int titleAlignment: BaseTitleBar.TitleAlignment.Center
	property var topSafeAreaMargin: plugin.safeAreaMargins.top

	height: contentLayout.height + topSafeAreaMargin

	function updateLeftAction() {
		if (leftActionStack.activeActionItem && typeof leftActionStack.activeActionItem.iconOnly !== "undefined") {
			leftActionStack.activeActionItem.iconOnly = titleText.implicitWidth > titleText.implicitAvailableWidth
		}
	}

	Rectangle {
		id: safeAreaBackground

		height: topSafeAreaMargin
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}

		property color baseColor: titleBar.color ? titleBar.color : Style.color.accent
		color: Constants.is_layout_android ? Qt.darker(baseColor, 1.2) : baseColor

		Behavior on color { ColorAnimation { duration: Constants.animation_duration } }
	}

	Rectangle {
		id: background

		anchors {
			top: safeAreaBackground.bottom
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}

		color: titleBar.color ? titleBar.color : Style.color.accent

		Behavior on color { ColorAnimation { duration: Constants.animation_duration } }
	}

	Column {
		id: contentLayout

		anchors {
			bottom: parent.bottom
			right: parent.right
			left: parent.left
		}

		Item {
			id: firstLine

			height: Style.dimens.titlebar_height
			width: parent.width

			Item {
				id: leftActionStack

				property var actionItem: leftAction
				property var activeActionItem

				width: activeActionItem ? activeActionItem.width : 0
				implicitWidth: activeActionItem ? activeActionItem.implicitWidth : 0
				anchors {
					top: parent.top
					left: parent.left
					bottom: parent.bottom
					leftMargin: Style.dimens.titlebar_padding
				}

				children: activeActionItem ? [activeActionItem] : []

				onActionItemChanged: leftActionStackAnimateOut.start()
				onActiveActionItemChanged: updateLeftAction()

				PropertyAnimation {
					id: leftActionStackAnimateOut
					target: leftActionStack
					property: "opacity"
					to: 0
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
					onStopped: { leftActionStack.activeActionItem = leftActionStack.actionItem; leftActionStackAnimateIn.start() }
				}
				PropertyAnimation {
					id: leftActionStackAnimateIn
					target: leftActionStack
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}

			GText {
				id: titleText

				readonly property var centerX: (parent.width / 2) - (width / 2)
				readonly property var leftX: leftActionStack.width + leftActionPadding + (1 + (leftActionStack.activeActionItem ? 1 : 0)) * Style.dimens.titlebar_padding

				readonly property var leftActionPadding: leftAction.iconOnly ? Style.dimens.titlebar_padding : 0
				readonly property var completePadding: (2 + (leftActionStack.activeActionItem ? 1 : 0) + (rightActionStack.activeActionItem ? 1 : 0)) * Style.dimens.titlebar_padding

				readonly property var implicitAvailableWidth: parent.width - leftActionStack.implicitWidth - rightActionStack.implicitWidth - completePadding
				readonly property var availableWidth: parent.width - leftActionStack.width - rightActionStack.width - completePadding - leftActionPadding

				height: Style.dimens.titlebar_height
				width: Math.min(implicitWidth, availableWidth)
				anchors {
					top: parent.top
					bottom: parent.bottom
				}
				x: titleAlignment === BaseTitleBar.TitleAlignment.Center ? Math.max(leftX, centerX) : leftX

				verticalAlignment: Text.AlignVCenter
				maximumLineCount: 1
				elide: Text.ElideRight
				wrapMode: Text.NoWrap
				font.bold: true
				textStyle: Style.text.header_inverse

				onTextChanged: updateLeftAction()

				Behavior on text {
					SequentialAnimation {
						PropertyAnimation {
							target: titleText
							property: "opacity"
							to: 0
							duration: Constants.animation_duration
							easing.type: Easing.InCubic
						}
						PropertyAction {
							target: titleText
							property: "text"
						}
						PropertyAnimation {
							target: titleText
							property: "opacity"
							to: 1
							duration: Constants.animation_duration
							easing.type: Easing.OutCubic
						}
					}
				}

				Behavior on x {
					NumberAnimation {
						from: parent.width * 0.75
						duration: Constants.animation_duration
						easing.type: Easing.OutQuart
					}
				}
			}

			Item {
				id: rightActionStack

				property var actionItem: rightAction
				property var activeActionItem

				width: activeActionItem ? activeActionItem.width : 0
				implicitWidth: activeActionItem ? activeActionItem.implicitWidth : 0
				anchors {
					top: parent.top
					right: parent.right
					bottom: parent.bottom
					rightMargin: Style.dimens.titlebar_padding
				}

				children: activeActionItem ? [activeActionItem] : []

				onActionItemChanged: rightActionStackAnimateOut.start()
				onActiveActionItemChanged: rightActionStackAnimateIn.start()

				PropertyAnimation {
					id: rightActionStackAnimateOut
					target: rightActionStack
					property: "opacity"
					to: 0
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
					onStopped: rightActionStack.activeActionItem = rightActionStack.actionItem
				}
				PropertyAnimation {
					id: rightActionStackAnimateIn
					target: rightActionStack
					property: "opacity"
					to: 1
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
				}
			}
		}

		Item {
			id: secondLine

			width: parent.width
			height: subTitleBarAction ? subTitleBarAction.height : 0

			data: subTitleBarAction ? subTitleBarAction : []
		}
	}
}
