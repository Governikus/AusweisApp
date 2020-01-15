/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtGraphicalEffects 1.0

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: titleBar
	height: actionRow.height + 2 * Style.dimens.titlebar_padding

	Accessible.role: Accessible.Grouping
	Accessible.name: qsTr("Titlebar") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This bar represents the navigation tree of the AusweisApp2.") + SettingsModel.translationTrigger
	activeFocusOnTab: true

	property var contentRoot
	signal rootClicked();

	property var rightMostAction: actionRow.lastAction

	function updateActions() {
		actionRow.children = [rootAction]
		addRecursive(contentRoot)
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
					//: LABEL DESKTOP_QML
					text: qsTr("Start") + SettingsModel.translationTrigger
					enabled: rightMostAction.rootEnabled
					helpTopic: "applicationOverview"

					onClicked: titleBar.rootClicked()
				}
			}
		}

		LinearGradient {
			visible: !actionRow.childrenFitSpace
			height: parent.height
			width: Constants.pane_padding
			anchors.right: rightTitleBarActions.left
			anchors.rightMargin: Style.dimens.titlebar_padding

			start: Qt.point(0, 0)
			end: Qt.point(Constants.pane_padding, 0)
			gradient: Gradient {
				GradientStop { position: 0.0; color: Style.color.transparent }
				GradientStop { position: 1.0; color: Constants.blue }
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
				source: "qrc:///images/settings_icon.svg"
				visible: rightMostAction.showSettings

				text: qsTr("Settings") + SettingsModel.translationTrigger
				Accessible.description: qsTr("Open settings view of %1").arg(Qt.application.name) + SettingsModel.translationTrigger

				onClicked: rightMostAction.settingsHandler()
			}

			TitleBarButton {
				id: helpButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/info_manual.svg"
				visible: rightMostAction.showHelp

				text: qsTr("Help") + SettingsModel.translationTrigger
				Accessible.description: qsTr("Open the online help of %1").arg(Qt.application.name) + SettingsModel.translationTrigger

				onClicked: ApplicationModel.openOnlineHelp(rightMostAction.helpTopic)
			}

			TitleBarButton {
				id: notifyButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/bell.svg"
				iconColor: notifications.iconColor
				visible: SettingsModel.showInAppNotifications

				text: qsTr("Notifications") + SettingsModel.translationTrigger
				Accessible.description: qsTr("Show the in-app notifications of %1").arg(Qt.application.name) + SettingsModel.translationTrigger

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
