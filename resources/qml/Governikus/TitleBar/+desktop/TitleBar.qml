import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.View 1.0


Item {
	id: titleBar
	height: actionRow.height + 2 * Constants.titlebar_padding

	focus: true
	KeyNavigation.tab: rootAction

	Accessible.role: Accessible.Grouping
	Accessible.name: qsTr("Titlebar")
	Accessible.description: qsTr("This bar represents the navigation tree of the AusweisApp2.")

	property Item navSuccessor: null

	property var contentRoot
	signal rootClicked();

	property var rightMostAction: actionRow.lastAction

	function updateActions() {
		actionRow.children = [rootAction]
		rootAction.KeyNavigation.tab = rightTitleBarActions.children[0]
		addRecursive(contentRoot)
	}

	function addRecursive(root) {
		for (var i in root.children) {
			var child = root.children[i]
			if (child.sectionPageTypeMarker && child.visible) {
				if (child.titleBarAction) {
					addAction(child.titleBarAction)
				}
				addRecursive(child)
			}
		}
	}

	function addAction(newAction) {
		rightMostAction.KeyNavigation.tab = newAction

		actionRow.children.push(newAction)
		newAction.customSubAction.KeyNavigation.tab = settingsButton
		newAction.KeyNavigation.tab = rightTitleBarActions.children[0]
	}

	Rectangle
	{
		anchors.fill: parent
		color: Constants.blue

		Row {
			id: actionRow

			height: rootAction.height
			anchors.left: parent.left
			anchors.leftMargin: Constants.titlebar_padding
			anchors.verticalCenter: parent.verticalCenter

			spacing: Constants.titlebar_spacing

			readonly property Item lastAction: children && children.length > 0 ? children[children.length - 1] : rootAction

			TitleBarAction {
				id: rootAction
				showArrow: false
				text: qsTr("Start")
				onClicked: titleBar.rootClicked()

				Component.onCompleted: {
					rootAction.customSubAction.KeyNavigation.tab = settingsButton
					rootAction.KeyNavigation.tab = rightTitleBarActions.children[0]
				}
			}
		}

		Row {
			id: rightTitleBarActions

			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.margins: Constants.titlebar_padding
			spacing: Constants.titlebar_spacing

			children: [
				rightMostAction.customSubAction,
				settingsButton,
				helpButton
			]

			TitleBarButton {
				id: settingsButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/settings_icon.svg"
				visible: rightMostAction.showSettings

				KeyNavigation.tab: helpButton
				Accessible.name: qsTr("Settings")

				onClicked: rightMostAction.settingsHandler()
			}

			TitleBarButton {
				id: helpButton

				height: rightTitleBarActions.height
				source: "qrc:///images/desktop/help_icon.svg"
				visible: rightMostAction.showHelp

				KeyNavigation.tab: titleBar.navSuccessor
				Accessible.name: qsTr("Help")

				onClicked: qmlExtension.openOnlineHelp(rightMostAction.helpTopic)
			}
		}
	}
}
