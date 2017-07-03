import QtQuick 2.5
import QtQuick.Controls 1.4 as QtQuickControls14
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import "global"

ApplicationWindow {
	id: appWindow
	visible: true
	width: 750 / 2 //Screen.desktopAvailableWidth
	height: 1334 / 2
	title: "Governikus AusweisApp2"
	color: Constants.background_color

	property int leftOverlayMargin: 0

	QtQuickControls14.Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.developerBuild ? plugin.doRefresh() : ""
	}

	header: TitleBar {
		id: titleBar

		titleBarOpacity: contentArea.getVisibleItem() && contentArea.getVisibleItem().stack.currentItem ? contentArea.getVisibleItem().stack.currentItem.titleBarOpacity : 1

		function pushTabBarSubView(sectionPage) {
			if (sectionPage) {
				titleBar.push(sectionPage.leftTitleBarAction,
					 sectionPage.headerTitleBarAction,
					 sectionPage.rightTitleBarAction,
					 sectionPage.titleBarColor)
			}
		}
	}

	overlay.modal: Item {
		anchors.fill: parent

		Rectangle {
			anchors.left: parent.left
			anchors.leftMargin: appWindow.leftOverlayMargin
			anchors.top: parent.top
			anchors.topMargin: Constants.titlebar_height
			height: appWindow.height - Constants.titlebar_height
			width: appWindow.width - anchors.leftMargin
			color: "black"
			opacity: parent.opacity * 0.5
		}
	}

	ContentArea {
		id: contentArea
		state: navBar.state
		anchors.left: PlatformConstants.leftNavigation ? navBar.right : parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: PlatformConstants.bottomNavigation ? navBar.top : parent.bottom
	}

	Navigation {
		id: navBar
		anchors.left: parent.left
		anchors.top: PlatformConstants.leftNavigation ? parent.top : undefined
		anchors.right: PlatformConstants.bottomNavigation ? parent.right : undefined
		anchors.bottom: parent.bottom
	}

	onClosing: {
		var activeStackView = contentArea.getVisibleItem().stack

		if (activeStackView.depth <= 1) {
			if (navBar.isOpen) {
				// FIXME:
				// On Android the closing event is caught by the Drawer, so the app is never quit
				plugin.fireQuitApplicationRequest()
				return
			}
			else {
				navBar.open()
			}
		}
		else if (activeStackView.currentItem.leftTitleBarAction) {
			if (navBar.isOpen) {
				navBar.close()
			}
			else {
				activeStackView.currentItem.leftTitleBarAction.clicked()
			}
		}

		close.accepted = false
	}
}
