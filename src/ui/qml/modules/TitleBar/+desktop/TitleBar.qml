/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type

Rectangle {
	id: root

	required property SectionPage contentItem
	readonly property TitleBarSettings currentSettings: contentItem.titleBarSettings
	property alias showPane: titlePane.visible
	readonly property string title: contentItem.title

	signal startClicked

	function setActiveFocus() {
		forceActiveFocus(Qt.MouseFocusReason);
	}

	color: Style.color.background
	height: titleBarColumn.height

	Column {
		id: titleBarColumn

		width: parent.width

		Item {
			id: firstRow

			height: rootAction.height + 2 * Style.dimens.titlebar_padding
			width: parent.width
			z: 1

			TitleBarAction {
				id: rootAction

				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.pane_padding
				anchors.verticalCenter: parent.verticalCenter
				enabled: root.currentSettings.startEnabled
				horizontalPadding: 0
				icon.source: "qrc:///images/desktop/home.svg"
				//: LABEL DESKTOP
				text: qsTr("Start page")

				onClicked: root.startClicked()
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
					source: "qrc:///images/desktop/material_settings.svg"
					text: qsTr("Settings")
					visible: root.currentSettings.showSettings

					onClicked: root.currentSettings.settingsClicked()
				}
				TitleBarButton {
					id: notifyButton

					Accessible.description: checked ?
					//: LABEL DESKTOP
					qsTr("Hide in-app notifications of %1").arg(Qt.application.name) :
					//: LABEL DESKTOP
					qsTr("Show in-app notifications of %1").arg(Qt.application.name)
					Accessible.role: Accessible.CheckBox
					checkable: true
					checked: notifications.visibleToUser
					height: rightTitleBarActions.height
					iconColor: notifications.iconColor
					source: notifications.unreadMessages ? "qrc:///images/desktop/notifications_on.svg" : "qrc:///images/desktop/notifications_off.svg"
					text: qsTr("Notifications")
					visible: SettingsModel.showInAppNotifications

					Accessible.onToggleAction: notifications.toggle()
					onClicked: notifications.toggle()
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
		TitlePane {
			id: titlePane

			visible: false
			width: parent.width
		}
		RowLayout {
			spacing: Style.dimens.pane_spacing
			visible: titlePane.visible
			width: parent.width

			NavigationAction {
				id: backAction

				Layout.fillHeight: true
				Layout.leftMargin: Style.dimens.pane_padding
				enabled: root.currentSettings.navigationEnabled
				type: root.currentSettings.navigationAction
				visible: root.currentSettings.navigationAction !== NavigationAction.None

				onClicked: root.currentSettings.navigationActionClicked()
			}
			GText {
				Accessible.role: Accessible.Heading
				Layout.maximumWidth: Number.POSITIVE_INFINITY
				Layout.rightMargin: x
				horizontalAlignment: Text.AlignHCenter
				maximumLineCount: 1
				text: root.title
				textStyle: Style.text.title
			}
		}
	}
	GSeparator {
		color: Style.color.pane.border.basic
		height: Style.dimens.border_width

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
}
