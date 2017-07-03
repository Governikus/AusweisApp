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

	readonly property var startTime : new Date().getTime()
	property var lastCloseInvocation: 0
	property int leftOverlayMargin: 0
	property var splasScreenClosed: false

	QtQuickControls14.Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.developerBuild ? plugin.doRefresh() : ""
	}

	header: TitleBar {
		id: titleBar
		visible: splasScreenClosed

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
		visible: splasScreenClosed
		anchors.left: parent.left
		anchors.top: PlatformConstants.leftNavigation ? parent.top : undefined
		anchors.right: PlatformConstants.bottomNavigation ? parent.right : undefined
		anchors.bottom: parent.bottom
	}

	onClosing: {
		var visibleItem = contentArea.getVisibleItem()

		if (visibleItem)
		{
			var activeStackView = visibleItem.stack

			if (activeStackView.depth <= 1 && (!activeStackView.currentItem.leftTitleBarAction || activeStackView.currentItem.leftTitleBarAction.state === "")) {
				var currentTime = new Date().getTime();
				if( currentTime - lastCloseInvocation < 1000 ) {
					plugin.fireQuitApplicationRequest()
					return
				}

				lastCloseInvocation = currentTime
				qmlExtension.showFeedback(qsTr("Press the back button twice to close the app."))
			}
			else if (activeStackView.currentItem.leftTitleBarAction) {
				if (navBar.isOpen) {
					navBar.close()
				}
				else {
					activeStackView.currentItem.leftTitleBarAction.clicked()
				}
			}
		}

		close.accepted = false
	}

	Rectangle {
		id: splashScreen
		anchors.fill: parent
		color: appWindow.color
		visible: !splasScreenClosed

		Image {
			source: "qrc:/images/npa.svg"
			height: appWindow.height * 0.42
			width: height
			fillMode: Image.PreserveAspectFit
			anchors.centerIn: parent
		}
	}


	function hideSplashScreen() {
		if(splasScreenClosed)
		{
			return;
		}

		contentArea.visible = false;

		var TIMEOUT = 2500;
		var remaining = startTime + TIMEOUT - new Date().getTime();
		var timer = Qt.createQmlObject("import QtQuick 2.0; Timer {}", appWindow);
		timer.interval = remaining > 0 ? remaining : 0;
		timer.repeat = false;
		timer.triggered.connect(function(){
			contentArea.visible = true
			splasScreenClosed = true
			if (!applicationModel.currentWorkflow) {
				navBar.lockedAndHidden = false
			}
		})
		timer.start();
	}
}
