/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
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
		Item {
			Layout.minimumHeight: titleBarNavigation.implicitHeight
			Layout.preferredWidth: titleBarNavigation.implicitWidth
			Layout.topMargin: Style.dimens.titlebar_padding

			TitleBarNavigation {
				id: titleBarNavigation

				Accessible.id: root.navigationAction ? root.navigationAction.Accessible.id : ""
				anchors.fill: parent
				navAction: root.navigationAction ? root.navigationAction.action : NavigationAction.Action.None
				visible: root.navigationAction ? root.navigationAction.enabled && (icon.source.toString() !== "" || text !== "") : false

				onClicked: root.navigationAction.clicked()
			}
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
			Loader {
				id: rightActionLoader

				function setRightAction(pRightAction) {
					if (SettingsModel.useAnimations && !ApplicationModel.screenReaderRunning) {
						rightActionStackAnimateOut.newSourceComponent = pRightAction;
						rightActionStackAnimateOut.start();
						return;
					}
					rightActionLoader.sourceComponent = pRightAction;
				}

				Layout.alignment: Qt.AlignRight | Qt.AlignTop
				visible: status === Loader.Ready

				PropertyAnimation {
					id: rightActionStackAnimateOut

					property Component newSourceComponent: null

					duration: Style.animation_duration
					easing.type: Easing.InCubic
					from: 1
					property: "opacity"
					target: rightActionLoader.item
					to: 0

					onStopped: {
						rightActionLoader.sourceComponent = newSourceComponent;
						if (newSourceComponent) {
							rightActionStackAnimateIn.start();
						}
					}
				}
				PropertyAnimation {
					id: rightActionStackAnimateIn

					duration: Style.animation_duration
					easing.type: Easing.OutCubic
					from: 0
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
