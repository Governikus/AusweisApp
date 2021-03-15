/*
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Navigation 1.0
import Governikus.View 1.0
import Governikus.FeedbackView 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Style 1.0

ApplicationWindow {
	id: appWindow

	readonly property alias ready: contentArea.ready

	visible: true

	// Workaround for qt 5.12 not calculating the highdpi scaling factor correctly. On some devices (like the pixel 3)
	// this leads to a small light stripe above the dark statusbar. By setting the background to black and filling the
	// rest of the window with the background color, it's still there but not noticeable.
	color: "#000000"
	Rectangle { anchors.fill: parent; color: Style.color.background }

	Component.onCompleted: {
		flags |= Qt.MaximizeUsingFullscreenGeometryHint
	}

	menuBar: TitleBar {
		id: titleBar

		property var currentSectionPage: if (contentArea) contentArea.currentSectionPage

		visible: !currentSectionPage || currentSectionPage.titleBarVisible

		navigationAction: currentSectionPage ? currentSectionPage.navigationAction : null
		title: currentSectionPage ? currentSectionPage.title : ""
		rightAction: currentSectionPage ?  currentSectionPage.rightTitleBarAction : null
		subTitleBarAction: currentSectionPage ?  currentSectionPage.subTitleBarAction : null
		color: currentSectionPage ? currentSectionPage.titleBarColor : null
		titleBarOpacity: currentSectionPage ? currentSectionPage.titleBarOpacity : 1
	}

	onClosing: { // back button pressed
		close.accepted = false

		if (contentArea.visibleItem) {
			if (contentArea.activeModule === UiModule.DEFAULT || SettingsModel.startupModule === UiModule.TUTORIAL) {
				var currentTime = new Date().getTime();
				if (currentTime - d.lastCloseInvocation < 1000) {
					plugin.fireQuitApplicationRequest()
					close.accepted = true
					return
				}

				d.lastCloseInvocation = currentTime
				//: INFO ANDROID IOS Hint that is shown if the users pressed the "back" button on the top-most navigation level for the first time (a second press closes the app).
				ApplicationModel.showFeedback(qsTr("To close the app, quickly press the back button twice."))
				return;
			}

			var activeStackView = contentArea.visibleItem.stackView
			var navigationAction = contentArea.currentSectionPage.navigationAction

			if (activeStackView.depth <= 1
					&& (!navigationAction || navigationAction.state !== "cancel")
					&& contentArea.activeModule !== UiModule.PROVIDER) {
				navBar.show(UiModule.DEFAULT)
			} else if (navigationAction) {
				navigationAction.clicked(undefined)
			}
		}
	}

	QtObject {
		id: d

		property var lastCloseInvocation: 0
	}

	Action {
		shortcut: "Ctrl+Alt+R"
		onTriggered: plugin.debugBuild ? plugin.doRefresh() : ""
	}

	Action {
		shortcut: "Escape"
		onTriggered: appWindow.close()
	}

	Connections {
		target: plugin
		enabled: contentArea.ready
		onFireApplicationActivated: feedback.showIfNecessary()
	}

	ContentArea {
		id: contentArea

		function reset() {
			visibleItem.currentSectionPage.firePopAll()
			visibleItem.currentSectionPage.reset()
		}

		anchors {
			left: parent.left
			top: parent.top
			right: parent.right
			bottom: navBar.top
			leftMargin: plugin.safeAreaMargins.left
			rightMargin: plugin.safeAreaMargins.right
			bottomMargin: (
				currentSectionPage && currentSectionPage.automaticSafeAreaMarginHandling && navBar.lockedAndHidden
			) ? plugin.safeAreaMargins.bottom : 0

			Behavior on bottomMargin {
				enabled: appWindow.ready
				NumberAnimation {duration: Constants.animation_duration}
			}
		}

		activeModule: navBar.activeModule
		onReadyChanged: feedback.showIfNecessary()
	}

	Navigation {
		id: navBar

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		onResetContentArea: contentArea.reset()
	}

	ConfirmationPopup {
		id: toast

		visible: ApplicationModel.feedback !== ""

		style: ApplicationModel.isScreenReaderRunning() ? ConfirmationPopup.PopupStyle.OkButton : ConfirmationPopup.PopupStyle.NoButtons
		closePolicy: Popup.NoAutoClose
		modal: ApplicationModel.isScreenReaderRunning()
		dim: true

		text: ApplicationModel.feedback

		onConfirmed: ApplicationModel.onShowNextFeedback()
	}

	StoreFeedbackPopup {
		id: feedback

		function showIfNecessary() {
			if (!ApplicationModel.currentWorkflow && !RemoteServiceModel.running && SettingsModel.requestStoreFeedback()) {
				SettingsModel.hideFutureStoreFeedbackDialogs()
				feedback.open()
			}
		}
	}
}
