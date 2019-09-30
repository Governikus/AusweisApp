/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

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

	Notifications {
		id: notifications

		anchors.right: parent.right
		anchors.top: parent.bottom

		onNewNotification: notifyButton.notify()
	}

	Rectangle
	{
		anchors.fill: parent
		color: Constants.blue

		FocusPoint {
			scope: titleBar
		}

		Row {
			id: actionRow

			height: rootAction.height
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.titlebar_padding
			anchors.verticalCenter: parent.verticalCenter

			spacing: Style.dimens.titlebar_padding

			readonly property Item lastAction: children && children.length > 0 ? children[children.length - 1] : rootAction

			TitleBarAction {
				id: rootAction

				activeFocusOnTab: true

				showArrow: false
				//: LABEL DESKTOP_QML
				text: qsTr("Start") + SettingsModel.translationTrigger
				enabled: rightMostAction.rootEnabled

				onClicked: titleBar.rootClicked()
			}
		}

		Row {
			id: rightTitleBarActions

			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.margins: Style.dimens.titlebar_padding
			spacing: Style.dimens.titlebar_padding

			children: [
				rightMostAction.customSubAction,
				settingsButton,
				helpButton,
				notifyButton
			]

			TitleBarButton {
				id: settingsButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/settings_icon.svg"
				visible: rightMostAction.showSettings

				activeFocusOnTab: true
				Accessible.name: qsTr("Settings") + SettingsModel.translationTrigger

				onClicked: rightMostAction.settingsHandler()
			}

			TitleBarButton {
				id: helpButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/info_manual.svg"
				visible: rightMostAction.showHelp

				activeFocusOnTab: true
				Accessible.name: qsTr("Help") + SettingsModel.translationTrigger

				onClicked: ApplicationModel.openOnlineHelp(rightMostAction.helpTopic)
			}

			TitleBarButton {
				id: notifyButton

				height: rightTitleBarActions.height
				source: notifications.icon
				visible: SettingsModel.showInAppNotifications

				activeFocusOnTab: true
				Accessible.name: qsTr("Notifications") + SettingsModel.translationTrigger

				onClicked: notifications.toggle()
			}
		}
	}
}
