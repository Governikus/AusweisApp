/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Governikus.Global
import Governikus.Style

Item {
	id: titleBar

	property NavigationAction navigationAction
	property var rightAction
	property bool showSeparator: false
	property bool smartEidUsed: false
	property alias title: titleText.text
	property alias titleBarOpacity: background.opacity
	property var topSafeAreaMargin: plugin.safeAreaMargins.top

	function setActiveFocus() {
		titleText.forceActiveFocus(Qt.MouseFocusReason);
	}

	height: contentLayout.implicitHeight + topSafeAreaMargin + Style.dimens.titlebar_padding

	Rectangle {
		id: safeAreaBackground

		color: smartEidUsed ? Style.color.card_smart : Style.color.card_eid
		height: topSafeAreaMargin

		Behavior on color {
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

		color: Style.color.background

		Behavior on color {
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
	ColumnLayout {
		id: contentLayout

		spacing: Constants.text_spacing
		width: Math.min(parent.width - 2 * Style.dimens.titlebar_padding - plugin.safeAreaMargins.left - plugin.safeAreaMargins.right, Style.dimens.max_text_width)

		anchors {
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
			horizontalCenterOffset: (plugin.safeAreaMargins.left - plugin.safeAreaMargins.right) / 2
		}
		TitleBarNavigation {
			id: leftAction

			Layout.minimumHeight: Style.dimens.small_icon_size
			action: titleBar.navigationAction ? titleBar.navigationAction.action : NavigationAction.Action.None
			enabled: titleBar.navigationAction ? titleBar.navigationAction.enabled : false

			onClicked: titleBar.navigationAction.clicked()
		}
		RowLayout {
			spacing: Constants.component_spacing

			GText {
				id: titleText

				Accessible.focusable: true
				Accessible.role: Accessible.Heading
				Layout.maximumWidth: Style.dimens.max_text_width
				elide: Text.ElideRight
				maximumLineCount: 2
				textStyle: Style.text.title

				Behavior on text {
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
			}
			GSpacer {
				Layout.fillWidth: true
			}
			Item {
				id: rightActionStack

				property var actionItem: rightAction
				property var activeActionItem

				Layout.alignment: Qt.AlignRight
				children: activeActionItem ? [activeActionItem] : []
				implicitHeight: activeActionItem ? activeActionItem.implicitHeight : 0
				implicitWidth: activeActionItem ? activeActionItem.implicitWidth : 0
				width: activeActionItem ? activeActionItem.implicitWidth : 0

				onActionItemChanged: rightActionStackAnimateOut.start()
				onActiveActionItemChanged: rightActionStackAnimateIn.start()

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
	GSeparator {
		visible: titleBar.showSeparator
		width: contentLayout.width

		anchors {
			horizontalCenter: contentLayout.horizontalCenter
			top: parent.bottom
		}
	}
}
