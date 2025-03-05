/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

Rectangle {
	id: root

	property alias enableTileStyle: titlePane.visible
	property NavigationAction navigationAction
	property var rightAction
	property alias showContent: contentLayout.visible
	property bool showSeparator: false
	property bool smartEidUsed: false
	property alias title: titleText.text
	property var topSafeAreaMargin: UiPluginModel.safeAreaMargins.top

	function setActiveFocus() {
		titleText.forceActiveFocus(Qt.MouseFocusReason);
	}

	color: Style.color.background
	height: Math.ceil((showContent ? contentLayout.implicitHeight : 0) + topSafeAreaMargin + titlePane.shadowHeight)

	Rectangle {
		id: safeAreaBackground

		color: root.smartEidUsed ? Style.color.card_smart : Style.color.background
		height: root.topSafeAreaMargin

		Behavior on color {
			ColorAnimation {
				duration: Style.animation_duration
			}
		}

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	TitlePane {
		id: titlePane

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			top: safeAreaBackground.bottom
		}
	}
	ColumnLayout {
		id: contentLayout

		spacing: Style.dimens.text_spacing
		width: Math.min(parent.width - 2 * Style.dimens.titlebar_padding - UiPluginModel.safeAreaMargins.left - UiPluginModel.safeAreaMargins.right, Style.dimens.max_text_width)

		anchors {
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
			horizontalCenterOffset: (UiPluginModel.safeAreaMargins.left - UiPluginModel.safeAreaMargins.right) / 2
		}
		TitleBarNavigation {
			id: leftAction

			Layout.minimumHeight: Style.dimens.small_icon_size
			Layout.topMargin: Style.dimens.titlebar_padding
			enabled: root.navigationAction ? root.navigationAction.enabled : false
			navAction: root.navigationAction ? root.navigationAction.action : NavigationAction.Action.None

			onClicked: root.navigationAction.clicked()
		}
		RowLayout {
			spacing: Style.dimens.pane_spacing

			GCrossBlendedText {
				id: titleText

				Accessible.focusable: true
				Accessible.role: Accessible.Heading
				Layout.maximumWidth: Style.dimens.max_text_width
				elide: Text.ElideRight
				maximumLineCount: 2
				textStyle: Style.text.title
			}
			GSpacer {
				Layout.fillWidth: true
			}
			Item {
				id: rightActionStack

				property var actionItem: root.rightAction
				property var activeActionItem

				Layout.alignment: Qt.AlignRight
				children: activeActionItem ? [activeActionItem] : []
				implicitHeight: activeActionItem ? activeActionItem.implicitHeight : 0
				implicitWidth: activeActionItem ? activeActionItem.implicitWidth : 0

				onActionItemChanged: rightActionStackAnimateOut.start()
				onActiveActionItemChanged: rightActionStackAnimateIn.start()

				PropertyAnimation {
					id: rightActionStackAnimateOut

					duration: Style.animation_duration
					easing.type: Easing.InCubic
					property: "opacity"
					target: rightActionStack
					to: 0

					onStopped: rightActionStack.activeActionItem = rightActionStack.actionItem
				}
				PropertyAnimation {
					id: rightActionStackAnimateIn

					duration: Style.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					target: rightActionStack
					to: 1
				}
			}
		}
		GSeparator {
			Layout.fillWidth: true
			opacity: root.showSeparator
		}
	}
}
