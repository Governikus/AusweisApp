/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel
import QtQuick.Layouts

Rectangle {
	id: titleBar

	property var contentRoot
	readonly property alias title: title.text

	signal rootClicked

	function setActiveFocus() {
		forceActiveFocus(Qt.MouseFocusReason);
	}
	function updateActions() {
		d.actions = [rootAction];
		d.addRecursive(contentRoot);
	}

	//: LABEL DESKTOP
	Accessible.name: qsTr("Title bar")
	Accessible.role: Accessible.Grouping
	activeFocusOnTab: true
	color: Style.color.background
	height: titleBarColumn.height

	QtObject {
		id: d

		property list<Item> actions
		readonly property TitleBarAction currentAction: actions && actions.length > 0 ? actions[actions.length - 1] : rootAction
		readonly property TitleBarAction prevAction: actions && actions.length > 1 ? actions[actions.length - 2] : rootAction

		function addRecursive(root) {
			for (let i in root.children) {
				let child = root.children[i];
				if (child.breadcrumpSearchPath && child.visible) {
					if (child instanceof SectionPage && child.titleBarAction) {
						actions.push(child.titleBarAction);
					}
					addRecursive(child);
				}
			}
		}
	}
	FocusPoint {
	}
	Column {
		id: titleBarColumn

		width: parent.width

		Item {
			id: firstRow

			height: rootAction.height + 2 * Style.dimens.titlebar_padding
			width: parent.width

			TitleBarAction {
				id: rootAction

				activeFocusOnTab: true
				anchors.left: parent.left
				anchors.leftMargin: Constants.pane_padding
				anchors.verticalCenter: parent.verticalCenter
				enabled: d.currentAction.rootEnabled
				horizontalPadding: 0
				icon.source: "qrc:///images/desktop/home.svg"
				//: LABEL DESKTOP
				text: qsTr("Start page")

				onClicked: titleBar.rootClicked()
			}
			Row {
				id: rightTitleBarActions

				anchors.bottom: parent.bottom
				anchors.margins: Style.dimens.titlebar_padding
				anchors.right: parent.right
				anchors.top: parent.top
				spacing: Style.dimens.titlebar_padding

				TitleBarButton {
					Accessible.description: qsTr("Open settings view of %1").arg(Qt.application.name)
					height: rightTitleBarActions.height
					source: "qrc:///images/desktop/material_settings_white.svg"
					text: qsTr("Settings")
					visible: d.currentAction.showSettings

					onClicked: d.currentAction.settingsHandler()
				}
				TitleBarButton {
					id: notifyButton

					Accessible.description: qsTr("Show in-app notifications of %1").arg(Qt.application.name)
					height: rightTitleBarActions.height
					iconColor: notifications.iconColor
					source: notifications.unreadMessages ? "qrc:///images/desktop/notifications_on.svg" : "qrc:///images/desktop/notifications_off.svg"
					text: qsTr("Notifications")
					visible: SettingsModel.showInAppNotifications

					onClicked: notifications.toggle()
				}
			}
		}
		TitlePane {
			id: titlePane

			visible: d.actions.length > 1
			width: parent.width
		}
		Row {
			height: title.height
			leftPadding: Constants.pane_padding
			visible: titlePane.visible
			width: parent.width

			RowLayout {
				height: parent.height
				width: 1.5 * Math.max(backAction.implicitWidth, Style.dimens.min_button_width)

				Item {
					Layout.alignment: Qt.AlignLeft
					Layout.fillHeight: true
					data: d.currentAction.customSubAction
				}
				NavigationAction {
					id: backAction

					Layout.alignment: Qt.AlignLeft
					Layout.fillHeight: true
					type: NavigationAction.Action.Back
					visible: !d.currentAction.customSubAction.visible

					onClicked: d.prevAction.clicked()
				}
			}
			GText {
				id: title

				Accessible.role: Accessible.Heading
				activeFocusOnTab: true
				text: d.currentAction.text
				textStyle: Style.text.title

				FocusFrame {
				}
			}
		}
	}
	GSeparator {
		color: Style.color.pane_border
		height: Style.dimens.border_width

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
	Notifications {
		id: notifications

		anchors.left: parent.right
		anchors.top: parent.top
		anchors.topMargin: firstRow.height

		onNewNotification: notifyButton.notify()
	}
}
