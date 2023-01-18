/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
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

	// Workaround for qt 5.12 not calculating the highdpi scaling factor correctly. On some devices (like the pixel 3)
	// this leads to a small light stripe above the dark statusbar. By setting the background to black and filling the
	// rest of the window with the background color, it's still there but not noticeable.
	color: "#000000"
	flags: Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint
	visible: true

	background: Rectangle {
		color: Style.color.background
	}
	menuBar: TitleBar {
		readonly property var currentSectionPage: contentArea.currentSectionPage
		readonly property bool isBackAction: navigationAction && navigationAction.action === NavigationAction.Action.Back

		color: currentSectionPage ? currentSectionPage.titleBarColor : null
		navigationAction: currentSectionPage ? currentSectionPage.navigationAction : null
		rightAction: currentSectionPage ? currentSectionPage.rightTitleBarAction : null
		title: currentSectionPage ? currentSectionPage.title : ""
		titleBarOpacity: currentSectionPage ? currentSectionPage.titleBarOpacity : 1
		visible: !currentSectionPage || currentSectionPage.titleBarVisible
	}

	Component.onCompleted: {
		feedback.showIfNecessary();
	}
	onClosing: pClose => {
		// back button pressed
		pClose.accepted = false;
		if (contentArea.visibleItem) {
			if (contentArea.activeModule === UiModule.DEFAULT) {
				var currentTime = new Date().getTime();
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
			var activeStackView = contentArea.visibleItem;
			var navigationAction = contentArea.currentSectionPage.navigationAction;
			if (activeStackView.depth <= 1 && (!navigationAction || navigationAction.action !== NavigationAction.Action.Cancel) && contentArea.activeModule !== UiModule.PROVIDER) {
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
			bottomMargin: !navigation.lockedAndHidden || !currentSectionPage ? 0 : (currentSectionPage.automaticSafeAreaMarginHandling ? plugin.safeAreaMargins.bottom : 0) + (currentSectionPage.hiddenNavbarPadding ? Style.dimens.navigation_bar_height : 0)
			left: parent.left
			leftMargin: plugin.safeAreaMargins.left
			right: parent.right
			rightMargin: plugin.safeAreaMargins.right
			top: parent.top

			Behavior on bottomMargin  {
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
			right: parent.right
		}
	}
	IosBackGestureMouseArea {
		anchors.fill: contentArea
		visible: Constants.is_layout_ios && menuBar.isBackAction

		onBackGestureTriggered: menuBar.navigationAction.clicked()
	}
	ConfirmationPopup {
		id: toast
		closePolicy: Popup.NoAutoClose
		dim: true
		modal: ApplicationModel.isScreenReaderRunning()
		style: ApplicationModel.isScreenReaderRunning() ? ConfirmationPopup.PopupStyle.OkButton : ConfirmationPopup.PopupStyle.NoButtons
		text: ApplicationModel.feedback
		visible: ApplicationModel.feedback !== ""

		onConfirmed: ApplicationModel.onShowNextFeedback()
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
