/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: titleBar

	property var color
	property alias contentHeight: contentLayout.height
	property bool enableTitleMoveAnimation: Constants.is_layout_ios
	property NavigationAction navigationAction
	property var rightAction
	property alias title: titleText.text
	property alias titleBarOpacity: background.opacity
	property var topSafeAreaMargin: plugin.safeAreaMargins.top

	function setActiveFocus() {
		titleText.forceActiveFocus(Qt.MouseFocusReason);
	}

	height: contentLayout.height + topSafeAreaMargin

	Rectangle {
		id: safeAreaBackground
		color: titleBar.color ? titleBar.color : Style.color.accent
		height: topSafeAreaMargin

		Behavior on color  {
			ColorAnimation {
				duration: Constants.animation_duration
			}
		}

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	Rectangle {
		id: background
		color: titleBar.color ? titleBar.color : Style.color.accent

		Behavior on color  {
			ColorAnimation {
				duration: Constants.animation_duration
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			top: safeAreaBackground.bottom
		}
	}
	Column {
		id: contentLayout
		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: plugin.safeAreaMargins.left
			right: parent.right
			rightMargin: plugin.safeAreaMargins.right
		}
		Item {
			id: firstLine
			height: Style.dimens.titlebar_height
			width: parent.width

			TitleBarNavigation {
				id: leftAction
				action: titleBar.navigationAction ? titleBar.navigationAction.action : NavigationAction.Action.None
				enabled: titleBar.navigationAction ? titleBar.navigationAction.enabled : false
				width: titleText.moreSpacePreferred ? minimumWidth : implicitWidth

				onClicked: titleBar.navigationAction.clicked()

				anchors {
					bottom: parent.bottom
					left: parent.left
					top: parent.top
				}
			}
			GText {
				id: titleText

				readonly property int availableWidth: parent.width - leftAction.width - rightActionStack.width
				readonly property int centerX: (parent.width / 2) - (width / 2)
				readonly property int implicitAvailableWidth: parent.width - leftAction.implicitWidth - rightActionStack.implicitWidth
				readonly property int leftX: leftAction.width
				readonly property bool moreSpacePreferred: implicitWidth > implicitAvailableWidth

				Accessible.focusable: true
				Accessible.role: Accessible.Heading
				elide: Text.ElideRight
				height: Style.dimens.titlebar_height
				leftPadding: Style.dimens.titlebar_padding
				maximumLineCount: 1
				rightPadding: Style.dimens.titlebar_padding
				textStyle: Style.text.header_inverse_highlight
				verticalAlignment: Text.AlignVCenter
				width: Math.min(implicitWidth, availableWidth)
				wrapMode: Text.NoWrap
				x: Math.max(leftX, centerX)

				Behavior on text  {
					SequentialAnimation {
						PropertyAnimation {
							duration: Constants.animation_duration
							easing.type: Easing.InCubic
							property: "opacity"
							target: titleText
							to: 0
						}
						PropertyAction {
							property: "text"
							target: titleText
						}
						PropertyAnimation {
							duration: Constants.animation_duration
							easing.type: Easing.OutCubic
							property: "opacity"
							target: titleText
							to: 1
						}
					}
				}
				Behavior on x  {
					enabled: enableTitleMoveAnimation

					NumberAnimation {
						duration: Constants.animation_duration
						easing.type: Easing.OutQuart
						from: parent.width * 0.75
					}
				}

				anchors {
					bottom: parent.bottom
					top: parent.top
				}
			}
			Item {
				id: rightActionStack

				property var actionItem: rightAction
				property var activeActionItem

				children: activeActionItem ? [activeActionItem] : []
				implicitWidth: activeActionItem ? activeActionItem.implicitWidth : 0
				width: activeActionItem ? activeActionItem.width : 0

				onActionItemChanged: rightActionStackAnimateOut.start()
				onActiveActionItemChanged: rightActionStackAnimateIn.start()

				anchors {
					bottom: parent.bottom
					right: parent.right
					rightMargin: Style.dimens.titlebar_padding
					top: parent.top
				}
				PropertyAnimation {
					id: rightActionStackAnimateOut
					duration: Constants.animation_duration
					easing.type: Easing.InCubic
					property: "opacity"
					target: rightActionStack
					to: 0

					onStopped: rightActionStack.activeActionItem = rightActionStack.actionItem
				}
				PropertyAnimation {
					id: rightActionStackAnimateIn
					duration: Constants.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					target: rightActionStack
					to: 1
				}
			}
		}
	}
}
