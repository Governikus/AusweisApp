/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.TitleBar
import Governikus.Navigation
import Governikus.View
import Governikus.FeedbackView
import Governikus.Type.ApplicationModel
import Governikus.Type.RemoteServiceModel
import Governikus.Type.SettingsModel
import Governikus.Type.UiModule
import Governikus.Style

ApplicationWindow {
	id: appWindow

	property var feedbackPopup: null

	// Workaround for qt 5.12 not calculating the highdpi scaling factor correctly. On some devices (like the pixel 3)
	// this leads to a small light stripe above the dark statusbar. By setting the background to black and filling the
	// rest of the window with the background color, it's still there but not noticeable.
	color: "#000000"
	flags: Qt.platform.os === "ios" ? Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint : Qt.Window
	visible: true

	background: Rectangle {
		color: Style.color.background
	}
	menuBar: TitleBar {
		readonly property var currentSectionPage: contentArea.currentSectionPage
		readonly property bool isBackAction: navigationAction && navigationAction.action === NavigationAction.Action.Back

		navigationAction: currentSectionPage ? currentSectionPage.navigationAction : null
		rightAction: currentSectionPage ? currentSectionPage.rightTitleBarAction : null
		showSeparator: currentSectionPage ? currentSectionPage.contentIsScrolled : false
		smartEidUsed: currentSectionPage ? currentSectionPage.smartEidUsed : false
		title: currentSectionPage ? currentSectionPage.title : ""
		titleBarOpacity: currentSectionPage ? currentSectionPage.titleBarOpacity : 1
		visible: !currentSectionPage || currentSectionPage.titleBarVisible
	}

	Component.onCompleted: {
		Style.dimens.screenHeight = Qt.binding(function () {
				return appWindow.height;
			});
		feedback.showIfNecessary();
	}
	onClosing: pClose => {
		// back button pressed
		pClose.accepted = false;
		if (contentArea.visibleItem) {
			if (contentArea.activeModule === UiModule.DEFAULT) {
				let currentTime = new Date().getTime();
				if (currentTime - d.lastCloseInvocation < 1000) {
					plugin.fireQuitApplicationRequest();
					pClose.accepted = true;
					return;
				}
				d.lastCloseInvocation = currentTime;
				//: INFO ANDROID IOS Hint that is shown if the users pressed the "back" button on the top-most navigation level for the first time (a second press closes the app).
				ApplicationModel.showFeedback(qsTr("To close the app, quickly press the back button twice."));
				return;
			}
			let activeStackView = contentArea.visibleItem;
			let navigationAction = contentArea.currentSectionPage.navigationAction;
			if (activeStackView.depth <= 1 && (!navigationAction || navigationAction.action !== NavigationAction.Action.Cancel)) {
				navigation.show(UiModule.DEFAULT);
			} else if (navigationAction) {
				navigationAction.clicked(undefined);
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
		function onFireApplicationActivated() {
			feedback.showIfNecessary();
		}

		target: plugin
	}
	Connections {
		function onFireShowRequest(pModule) {
			switch (pModule) {
			case UiModule.CURRENT:
				break;
			case UiModule.UPDATEINFORMATION:
				navigation.show(UiModule.DEFAULT);
				break;
			case UiModule.TUTORIAL:
				navigation.show(UiModule.HELP);
				break;
			case UiModule.IDENTIFY:
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE) {
					navigation.show(UiModule.SELF_AUTHENTICATION);
					break;
				}
			default:
				navigation.show(pModule);
				break;
			}
		}

		target: plugin
	}
	ContentArea {
		id: contentArea

		function reset() {
			currentSectionPage.popAll();
			currentSectionPage.reset();
		}

		activeModule: navigation.activeModule

		anchors {
			bottom: navigation.top
			bottomMargin: !navigation.lockedAndHidden || !currentSectionPage ? 0 : plugin.safeAreaMargins.bottom + (currentSectionPage.hiddenNavbarPadding ? navigation.height : 0)
			left: parent.left
			leftMargin: plugin.safeAreaMargins.left
			right: parent.right
			rightMargin: plugin.safeAreaMargins.right
			top: parent.top

			Behavior on bottomMargin {
				enabled: !ApplicationModel.isScreenReaderRunning()

				NumberAnimation {
					duration: Constants.animation_duration
				}
			}
		}
	}
	Navigation {
		id: navigation

		onResetContentArea: contentArea.reset()

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: plugin.safeAreaMargins.left
			right: parent.right
			rightMargin: plugin.safeAreaMargins.right
		}
	}
	IosBackGestureMouseArea {
		anchors.fill: contentArea
		visible: Constants.is_layout_ios && menuBar.isBackAction

		onBackGestureTriggered: menuBar.navigationAction.clicked()
	}
	Connections {
		function onFireFeedbackChanged() {
			if (feedbackPopup) {
				feedbackPopup.close();
				feedbackPopup.destroy();
				feedbackPopup = null;
			}
			if (ApplicationModel.feedback !== "") {
				feedbackPopup = toast.createObject(appWindow, {
						"text": ApplicationModel.feedback
					});
				feedbackPopup.open();
			}
		}

		target: ApplicationModel
	}
	Component {
		id: toast

		ConfirmationPopup {
			closePolicy: Popup.NoAutoClose
			dim: true
			modal: ApplicationModel.isScreenReaderRunning()
			style: ApplicationModel.isScreenReaderRunning() ? ConfirmationPopup.PopupStyle.OkButton : ConfirmationPopup.PopupStyle.NoButtons

			onConfirmed: ApplicationModel.onShowNextFeedback()
		}
	}
	StoreFeedbackPopup {
		id: feedback

		function showIfNecessary() {
			if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE && !RemoteServiceModel.running && SettingsModel.requestStoreFeedback()) {
				SettingsModel.hideFutureStoreFeedbackDialogs();
				feedback.open();
			}
		}
	}
}
