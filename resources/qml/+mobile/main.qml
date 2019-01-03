import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Window 2.2

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Navigation 1.0
import Governikus.SplashScreen 1.0
import Governikus.View 1.0
import Governikus.FeedbackView 1.0
import Governikus.Type.ApplicationModel 1.0

ApplicationWindow {
	id: appWindow
	visible: true
	width: 750 / 2 //Screen.desktopAvailableWidth
	height: 1334 / 2
	title: "Governikus AusweisApp2"
	color: Constants.background_color

	property var lastCloseInvocation: 0

	Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.developerBuild ? plugin.doRefresh() : ""
	}

	Action {
		shortcut: "Escape"
		onTriggered: appWindow.close()
	}

	menuBar: TitleBar {
		id: titleBar
		visible: !splashScreen.visible && contentArea.state !== "tutorial"

		titleBarOpacity: contentArea.visibleItem && contentArea.visibleItem.stack.currentItem ? contentArea.visibleItem.stack.currentItem.titleBarOpacity : 1

		property var currentSectionPage: if (contentArea) contentArea.currentSectionPage

		leftAction: if (currentSectionPage) currentSectionPage.leftTitleBarAction
		titleItem: if (currentSectionPage) currentSectionPage.headerTitleBarAction
		rightAction: if (currentSectionPage) currentSectionPage.rightTitleBarAction
		subTitleBarAction: if (currentSectionPage) currentSectionPage.subTitleBarAction
		color: if (currentSectionPage) currentSectionPage.titleBarColor
	}

	ContentAreaLoader {
		id: contentArea
		state: navBar.state
		anchors.left: Constants.leftNavigation ? navBar.right : parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		anchors.bottom: Constants.bottomNavigation ? navBar.top : parent.bottom
	}

	Navigation {
		id: navBar
		visible: !splashScreen.visible
		anchors.left: parent.left
		anchors.top: Constants.leftNavigation ? parent.top : undefined
		anchors.right: Constants.bottomNavigation ? parent.right : undefined
		anchors.bottom: parent.bottom
	}

	onClosing: { // back button pressed
		if (contentArea.visibleItem)
		{
			var activeStackView = contentArea.visibleItem.stack
			var leftTitleBarAction = activeStackView.currentItem.leftTitleBarAction

			if (activeStackView.depth <= 1
			    && (!leftTitleBarAction || leftTitleBarAction.state === "")
			    && contentArea.state != "tutorial") // Don't go back to "identify" section page when tutorial is active
				{
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
			else if (leftTitleBarAction) {
				if (navBar.isOpen) {
					navBar.close()
				}
				else if (leftTitleBarAction.state !== "hidden") {
					leftTitleBarAction.clicked(undefined)
				}
			}
		}

		close.accepted = false
	}

	StoreFeedbackPopup {
		id: feedback

		x: (appWindow.width - width) / 2
		y: (appWindow.height - height) / 2

		property alias ready: contentArea.ready
		onReadyChanged: {
			if (ApplicationModel.areStoreFeedbackDialogConditionsMet()) {
				ApplicationModel.hideFutureStoreFeedbackDialogs()
				feedback.open()
			}
		}
	}

	SplashScreen {
		id: splashScreen
		color: appWindow.color

		property alias ready: contentArea.ready
		onReadyChanged: {
			splashScreen.hide()
			if (!ApplicationModel.currentWorkflow && !settingsModel.showTutorialOnStart) {
				navBar.lockedAndHidden = false
			}
		}
	}
}
