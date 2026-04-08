/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
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
	property Component rightAction
	required property real safeAreaTopMargin
	property alias showContent: contentLayout.visible
	property bool showSeparator: false
	property alias title: titleText.text

	function setActiveFocus() {
		titleText.forceActiveFocus(Qt.MouseFocusReason);
	}

	color: Style.color.background
	height: Math.ceil((showContent ? contentLayout.implicitHeight : 0) + safeAreaTopMargin + titlePane.shadowHeight)

	onRightActionChanged: rightActionLoader.setRightAction(rightAction)

	MouseArea {
		anchors.fill: parent
	}
	Rectangle {
		id: statusBarBackground

		color: Style.color.background
		height: root.safeAreaTopMargin

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
			top: statusBarBackground.bottom
		}
	}
	ColumnLayout {
		id: contentLayout

		spacing: Style.dimens.text_spacing
		width: Math.min(parent.width - 2 * Style.dimens.titlebar_padding, Style.dimens.max_text_width)

		anchors {
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}
		TitleBarNavigation {
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
			Loader {
				id: rightActionLoader

				function setRightAction(pRightAction) {
					if (SettingsModel.useAnimations && pRightAction !== null) {
						rightActionStackAnimateOut.newSourceComponent = pRightAction;
						rightActionStackAnimateOut.start();
						return;
					}
					rightActionLoader.sourceComponent = pRightAction;
				}

				Layout.alignment: Qt.AlignRight | Qt.AlignTop

				PropertyAnimation {
					id: rightActionStackAnimateOut

					property Component newSourceComponent: null

					duration: Style.animation_duration
					easing.type: Easing.InCubic
					property: "opacity"
					target: rightActionLoader.item
					to: 0

					onStopped: {
						rightActionLoader.sourceComponent = newSourceComponent;
						if (newSourceComponent !== null) {
							rightActionStackAnimateIn.start();
						}
					}
				}
				PropertyAnimation {
					id: rightActionStackAnimateIn

					duration: Style.animation_duration
					easing.type: Easing.OutCubic
					property: "opacity"
					target: rightActionLoader.item
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
