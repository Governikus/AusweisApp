/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: titleBar

	function setActiveFocus() {
		titleText.forceActiveFocus(Qt.MouseFocusReason)
	}

	property alias contentHeight: contentLayout.height
	property alias titleBarOpacity: background.opacity

	property alias title: titleText.text
	property NavigationAction navigationAction
	property var rightAction

	property var color
	property bool enableTitleMoveAnimation: Constants.is_layout_ios
	property var topSafeAreaMargin: plugin.safeAreaMargins.top

	height: contentLayout.height + topSafeAreaMargin

	Rectangle {
		id: safeAreaBackground

		height: topSafeAreaMargin
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}

		color: titleBar.color ? titleBar.color : Style.color.accent

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
			leftMargin: plugin.safeAreaMargins.left
			rightMargin: plugin.safeAreaMargins.right
		}

		Item {
			id: firstLine

			height: Style.dimens.titlebar_height
			width: parent.width

			TitleBarNavigation {
				id: leftAction

				anchors {
					top: parent.top
					left: parent.left
					bottom: parent.bottom
				}
				width: titleText.moreSpacePreferred ? minimumWidth : implicitWidth

				action: titleBar.navigationAction ? titleBar.navigationAction.action : NavigationAction.Action.None
				enabled: titleBar.navigationAction ? titleBar.navigationAction.enabled : false

				onClicked: titleBar.navigationAction.clicked()
			}

			GText {
				id: titleText

				readonly property int centerX: (parent.width / 2) - (width / 2)
				readonly property int leftX: leftAction.width

				readonly property int implicitAvailableWidth: parent.width - leftAction.implicitWidth - rightActionStack.implicitWidth
				readonly property int availableWidth: parent.width - leftAction.width - rightActionStack.width
				readonly property bool moreSpacePreferred: implicitWidth > implicitAvailableWidth

				Accessible.role: Accessible.Heading
				Accessible.focusable: true

				height: Style.dimens.titlebar_height
				width: Math.min(implicitWidth, availableWidth)
				anchors {
					top: parent.top
					bottom: parent.bottom
				}
				x: Math.max(leftX, centerX)

				leftPadding: Style.dimens.titlebar_padding
				rightPadding: Style.dimens.titlebar_padding
				verticalAlignment: Text.AlignVCenter
				maximumLineCount: 1
				elide: Text.ElideRight
				wrapMode: Text.NoWrap
				font.bold: true
				textStyle: Style.text.header_inverse

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
					enabled: enableTitleMoveAnimation
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
	}
}
