import QtQuick 2.5
import QtQuick.Controls 1.4 as QtQuickControls14
import QtQuick.Controls 2.0
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Navigation 1.0
import Governikus.SplashScreen 1.0

ApplicationWindow {
	id: appWindow
	visible: true
	width: 750 / 2 //Screen.desktopAvailableWidth
	height: 1334 / 2
	title: "Governikus AusweisApp2"
	color: Constants.background_color

	property var lastCloseInvocation: 0
	property int leftOverlayMargin: 0


	QtQuickControls14.Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.developerBuild ? plugin.doRefresh() : ""
	}

	header: TitleBar {
		id: titleBar
		visible: !splashScreen.visible

		titleBarOpacity: contentArea.visibleItem && contentArea.visibleItem.stack.currentItem ? contentArea.visibleItem.stack.currentItem.titleBarOpacity : 1

		property var currentSectionPage: if (contentArea) contentArea.currentSectionPage

		leftAction: if (currentSectionPage) currentSectionPage.leftTitleBarAction
		titleItem: if (currentSectionPage) currentSectionPage.headerTitleBarAction
		rightAction: if (currentSectionPage) currentSectionPage.rightTitleBarAction
		subTitleBarAction: if (currentSectionPage) currentSectionPage.subTitleBarAction
		color: if (currentSectionPage) currentSectionPage.titleBarColor


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

	ContentAreaLoader {
		id: contentArea
		state: navBar.state
		anchors.left: PlatformConstants.leftNavigation ? navBar.right : parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: PlatformConstants.bottomNavigation ? navBar.top : parent.bottom
	}

	Navigation {
		id: navBar
		visible: !splashScreen.visible
		anchors.left: parent.left
		anchors.top: PlatformConstants.leftNavigation ? parent.top : undefined
		anchors.right: PlatformConstants.bottomNavigation ? parent.right : undefined
		anchors.bottom: parent.bottom
	}

	onClosing: {
		if (contentArea.visibleItem)
		{
			var activeStackView = contentArea.visibleItem.stack

			if (activeStackView.depth <= 1 && (!activeStackView.currentItem.leftTitleBarAction || activeStackView.currentItem.leftTitleBarAction.state === "")) {
				if (contentArea.state != "identify") {
					navBar.state = "identify"
					navBar.currentIndex = 0
				} else {
					var currentTime = new Date().getTime();
					if( currentTime - lastCloseInvocation < 1000 ) {
						plugin.fireQuitApplicationRequest()
						return
					}

					lastCloseInvocation = currentTime
					qmlExtension.showFeedback(qsTr("To close the app, quickly press the back button twice."))
				}
			}
			else if (activeStackView.currentItem.leftTitleBarAction) {
				if (navBar.isOpen) {
					navBar.close()
				}
				else {
					activeStackView.currentItem.leftTitleBarAction.clicked(undefined)
				}
			}
		}

		close.accepted = false
	}

	SplashScreen {
		id: splashScreen
		color: appWindow.color

		property alias ready: contentArea.ready
		onReadyChanged: {
			splashScreen.hide()
			if (!applicationModel.currentWorkflow) {
				navBar.lockedAndHidden = false
			}
		}
	}
}
