/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: titleBar

	function setActiveFocus() {
		forceActiveFocus(Qt.MouseFocusReason)
	}

	height: actionRow.height + 2 * Style.dimens.titlebar_padding

	Accessible.role: Accessible.Grouping

	activeFocusOnTab: true

	property var contentRoot
	signal rootClicked()

	property var rightMostAction: actionRow.lastAction

	function updateActions() {
		actionRow.children = [rootAction]
		addRecursive(contentRoot)

		let visibleActionsCount = getVisibleTitleBarActionCount()
		Accessible.name = buildAccessibleName(visibleActionsCount)
		updateListIndexAndLength(visibleActionsCount)
	}

	function addRecursive(root) {
		for (var i in root.children) {
			var child = root.children[i]
			if (child.sectionPageTypeMarker && child.visible) {
				if (child.titleBarAction) {
					actionRow.children.push(child.titleBarAction)
				}
				addRecursive(child)
			}
		}
	}

	function buildAccessibleName(visibleActionsCount) {
		//: LABEL DESKTOP
		var accessible_name = qsTr("Navigation bar") + ", "
		//: LABEL DESKTOP
		accessible_name += qsTr("List") + ", "
		if (actionRow.children.length > 1) {
			//: LABEL DESKTOP
			accessible_name += qsTr("%1 elements").arg(visibleActionsCount)
		} else {
			//: LABEL DESKTOP
			accessible_name += qsTr("1 element")
		}
		return accessible_name
	}

	function updateListIndexAndLength(visibleActionsCount) {
		var idx = 1
		for (var i = 0; i < actionRow.children.length; i++) {
			if (actionRow.children[i].visible) {
				actionRow.children[i].list_index = idx;
				actionRow.children[i].list_length = visibleActionsCount;
				idx += 1
			}
		}
	}

	function getVisibleTitleBarActionCount() {
		var count = 0
		for (var i = 0; i < actionRow.children.length; i++) {
			if (actionRow.children[i].visible) {
				count += 1
			}
		}
		return count
	}

	Rectangle
	{
		anchors.fill: parent
		color: Style.color.navigation

		GSeparator {
			anchors {
				bottom: parent.bottom
				left: parent.left
				right: parent.right
			}

			height: Style.dimens.high_contrast_item_border
			color: Style.color.high_contrast_item_border
		}

		FocusPoint {
			scope: titleBar
			isOnLightBackground: false
		}

		Item {
			height: parent.height
			anchors.left: parent.left
			anchors.right: rightTitleBarActions.left
			anchors.rightMargin: Style.dimens.titlebar_padding

			clip: true

			Row {
				id: actionRow

				height: rootAction.height
				anchors.leftMargin: Style.dimens.titlebar_padding
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: parent.left
				anchors.right: parent.right

				spacing: Style.dimens.titlebar_padding

				readonly property bool childrenFitSpace: childrenRect.width <= width
				readonly property Item lastAction: children && children.length > 0 ? children[children.length - 1] : rootAction

				TitleBarAction {
					id: rootAction

					activeFocusOnTab: true

					showArrow: false
					//: LABEL DESKTOP
					text: qsTr("Start page")
					active: rightMostAction.rootEnabled
					helpTopic: "applicationOverview"

					onClicked: titleBar.rootClicked()
				}
			}
		}

		Rectangle {
			visible: !actionRow.childrenFitSpace
			height: parent.height
			width: Constants.pane_padding
			anchors.right: rightTitleBarActions.left
			anchors.rightMargin: Style.dimens.titlebar_padding

			gradient: Gradient {
				orientation: Gradient.Horizontal
				GradientStop { position: 0.0; color: Style.color.transparent }
				GradientStop { position: 1.0; color: Style.color.navigation }
			}
		}

		Row {
			id: rightTitleBarActions

			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.margins: Style.dimens.titlebar_padding
			spacing: Style.dimens.titlebar_padding

			Item {
				height: parent.height
				width: childrenRect.width

				data: rightMostAction.customSubAction
			}

			TitleBarButton {
				id: settingsButton

				height: rightTitleBarActions.height
				source: "qrc:///images/material_settings.svg"
				visible: rightMostAction.showSettings

				text: qsTr("Settings")
				Accessible.description: qsTr("Open settings view of %1").arg(Qt.application.name)

				onClicked: rightMostAction.settingsHandler()
			}

			TitleBarButton {
				id: helpButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/material_menu_book.svg"
				visible: rightMostAction.showHelp

				text: qsTr("Open online help in browser")
				Accessible.description: qsTr("Open online help of %1 in browser").arg(Qt.application.name)

				onClicked: ApplicationModel.openOnlineHelp(rightMostAction.helpTopic)
			}

			TitleBarButton {
				id: notifyButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/material_notifications.svg"
				iconColor: notifications.iconColor
				visible: SettingsModel.showInAppNotifications

				text: qsTr("Notifications")
				Accessible.description: qsTr("Show in-app notifications of %1").arg(Qt.application.name)

				onClicked: notifications.toggle()
			}
		}
	}

	Notifications {
		id: notifications

		z: -1 // Draw below the title bar, but place Notifications after the notifyButton, so that the tab order makes sense
		anchors.right: parent.right
		anchors.top: parent.bottom

		onNewNotification: notifyButton.notify()
	}
}
