/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.UpdateView
import Governikus.View
import Governikus.Type

Rectangle {
	id: root

	required property SectionPage contentItem
	readonly property TitleBarSettings currentSettings: contentItem.titleBarSettings
	property alias showPane: titlePane.visible
	readonly property string title: contentItem.title

	signal showUpdate
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
				//: DESKTOP
				text: qsTr("Start page")

				onClicked: root.startClicked()
			}
			GLink {
				id: updateButton

				Accessible.name: text + (enabled ? "" : ". " + toolTip.text)
				enabled: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE
				font.underline: true
				//: DESKTOP %1 will be replaced with an application version number
				text: qsTr("Update available (version %1)").arg(SettingsModel.appUpdateData.version)
				visible: SettingsModel.appUpdateData.updateAvailable && !(root.contentItem instanceof UpdateView)

				onClicked: root.showUpdate()

				anchors {
					right: rightTitleBarActions.left
					top: rootAction.top
					topMargin: rootAction.baselineOffset - baselineOffset
				}
				ToolTip {
					id: toolTip

					delay: Style.toolTipDelay
					//: DESKTOP
					text: qsTr("The update can only be performed after the current operation has been completed.")
					visible: !updateButton.enabled && updateButton.hovered
				}
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
					//: DESKTOP
					qsTr("Hide in-app notifications of %1").arg(Qt.application.name) :
					//: DESKTOP
					qsTr("Show in-app notifications of %1").arg(Qt.application.name)
					Accessible.role: {
						if ("Switch" in Accessible) {
							return Accessible.Switch; // qmllint disable missing-property
						}
						return Accessible.CheckBox;
					}
					checkable: true
					checked: notifications.visibleToUser
					height: rightTitleBarActions.height
					iconColor: notifications.iconColor
					source: notifications.unreadMessages ? "qrc:///images/desktop/notifications_on.svg" : "qrc:///images/desktop/notifications_off.svg"
					text: qsTr("Notifications")
					visible: SettingsModel.developerOptions && SettingsModel.showInAppNotifications

					Accessible.onToggleAction: notifications.toggle()
					onClicked: notifications.toggle()
				}
			}
			Notifications {
				id: notifications

				anchors.left: parent.right
				anchors.top: parent.top
				anchors.topMargin: firstRow.height
				visible: notifyButton.visible

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
				Layout.fillHeight: true
				Layout.leftMargin: Style.dimens.pane_padding
				enabled: root.currentSettings.navigationEnabled
				type: root.currentSettings.navigationAction
				visible: root.currentSettings.navigationAction !== NavigationAction.Action.None

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
		color: Style.color.pane.border.basic_unchecked
		height: Style.dimens.border_width

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
}
