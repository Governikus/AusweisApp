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

	property var contentRoot
	property var rightMostAction: actionRow.lastAction

	signal rootClicked

	function addRecursive(root) {
		for (var i in root.children) {
			var child = root.children[i];
			if (child.sectionPageTypeMarker && child.visible) {
				if (child.titleBarAction) {
					actionRow.children.push(child.titleBarAction);
				}
				addRecursive(child);
			}
		}
	}
	function buildAccessibleName(visibleActionsCount) {
		//: LABEL DESKTOP
		var accessible_name = qsTr("Navigation bar") + ", ";
		//: LABEL DESKTOP
		accessible_name += qsTr("List") + ", ";
		if (actionRow.children.length > 1) {
			//: LABEL DESKTOP
			accessible_name += qsTr("%1 elements").arg(visibleActionsCount);
		} else {
			//: LABEL DESKTOP
			accessible_name += qsTr("1 element");
		}
		return accessible_name;
	}
	function getVisibleTitleBarActionCount() {
		var count = 0;
		for (var i = 0; i < actionRow.children.length; i++) {
			if (actionRow.children[i].visible) {
				count += 1;
			}
		}
		return count;
	}
	function setActiveFocus() {
		forceActiveFocus(Qt.MouseFocusReason);
	}
	function updateActions() {
		actionRow.children = [rootAction];
		addRecursive(contentRoot);
		let visibleActionsCount = getVisibleTitleBarActionCount();
		Accessible.name = buildAccessibleName(visibleActionsCount);
		updateListIndexAndLength(visibleActionsCount);
	}
	function updateListIndexAndLength(visibleActionsCount) {
		var idx = 1;
		for (var i = 0; i < actionRow.children.length; i++) {
			if (actionRow.children[i].visible) {
				actionRow.children[i].list_index = idx;
				actionRow.children[i].list_length = visibleActionsCount;
				idx += 1;
			}
		}
	}

	Accessible.role: Accessible.Grouping
	activeFocusOnTab: true
	height: actionRow.height + 2 * Style.dimens.titlebar_padding

	Rectangle {
		anchors.fill: parent
		color: Style.color.navigation

		GSeparator {
			color: Style.color.high_contrast_item_border
			height: Style.dimens.high_contrast_item_border

			anchors {
				bottom: parent.bottom
				left: parent.left
				right: parent.right
			}
		}
		FocusPoint {
			isOnLightBackground: false
			scope: titleBar
		}
		Item {
			anchors.left: parent.left
			anchors.right: rightTitleBarActions.left
			anchors.rightMargin: Style.dimens.titlebar_padding
			clip: true
			height: parent.height

			Row {
				id: actionRow

				readonly property bool childrenFitSpace: childrenRect.width <= width
				readonly property Item lastAction: children && children.length > 0 ? children[children.length - 1] : rootAction

				anchors.left: parent.left
				anchors.leftMargin: Style.dimens.titlebar_padding
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				height: rootAction.height
				spacing: Style.dimens.titlebar_padding

				TitleBarAction {
					id: rootAction
					active: rightMostAction.rootEnabled
					activeFocusOnTab: true
					helpTopic: "applicationOverview"
					showArrow: false
					//: LABEL DESKTOP
					text: qsTr("Start page")

					onClicked: titleBar.rootClicked()
				}
			}
		}
		Rectangle {
			anchors.right: rightTitleBarActions.left
			anchors.rightMargin: Style.dimens.titlebar_padding
			height: parent.height
			visible: !actionRow.childrenFitSpace
			width: Constants.pane_padding

			gradient: Gradient {
				orientation: Gradient.Horizontal

				GradientStop {
					color: Style.color.transparent
					position: 0.0
				}
				GradientStop {
					color: Style.color.navigation
					position: 1.0
				}
			}
		}
		Row {
			id: rightTitleBarActions
			anchors.bottom: parent.bottom
			anchors.margins: Style.dimens.titlebar_padding
			anchors.right: parent.right
			anchors.top: parent.top
			spacing: Style.dimens.titlebar_padding

			Item {
				data: rightMostAction.customSubAction
				height: parent.height
				width: childrenRect.width
			}
			TitleBarButton {
				id: settingsButton
				Accessible.description: qsTr("Open settings view of %1").arg(Qt.application.name)
				height: rightTitleBarActions.height
				source: "qrc:///images/material_settings.svg"
				text: qsTr("Settings")
				visible: rightMostAction.showSettings

				onClicked: rightMostAction.settingsHandler()
			}
			TitleBarButton {
				id: helpButton
				Accessible.description: qsTr("Open online help of %1 in browser").arg(Qt.application.name)
				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/material_menu_book.svg"
				text: qsTr("Open online help in browser")
				visible: rightMostAction.showHelp

				onClicked: ApplicationModel.openOnlineHelp(rightMostAction.helpTopic)
			}
			TitleBarButton {
				id: notifyButton
				Accessible.description: qsTr("Show in-app notifications of %1").arg(Qt.application.name)
				height: rightTitleBarActions.height
				iconColor: notifications.iconColor
				source: "qrc:///images/desktop/material_notifications.svg"
				text: qsTr("Notifications")
				visible: SettingsModel.showInAppNotifications

				onClicked: notifications.toggle()
			}
		}
	}
	Notifications {
		id: notifications
		anchors.right: parent.right
		anchors.top: parent.bottom
		z: -1 // Draw below the title bar, but place Notifications after the notifyButton, so that the tab order makes sense

		onNewNotification: notifyButton.notify()
	}
}
