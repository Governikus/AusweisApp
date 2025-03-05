/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.TitleBar
import Governikus.Navigation
import Governikus.View
import Governikus.FeedbackView
import Governikus.Type
import Governikus.Style

ApplicationWindow {
	id: root

	property var feedbackPopup: null
	readonly property Navigation navigationInstance: navigation

	flags: Qt.platform.os === "ios" ? Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint : Qt.Window
	visible: true

	background: Rectangle {
		color: Style.color.background
	}
	footer: Navigation {
		id: navigation

		onResetContentArea: contentArea.reset()

		anchors {
			left: parent.left
			leftMargin: UiPluginModel.safeAreaMargins.left
			right: parent.right
			rightMargin: UiPluginModel.safeAreaMargins.right
		}
	}
	menuBar: TitleBar {
		id: titleBar

		readonly property var currentSectionPage: contentArea.currentSectionPage
		readonly property bool isBackAction: navigationAction && navigationAction.action === NavigationAction.Action.Back

		enableTileStyle: currentSectionPage ? currentSectionPage.enableTileStyle : false
		navigationAction: currentSectionPage ? currentSectionPage.navigationAction : null
		rightAction: currentSectionPage ? currentSectionPage.rightTitleBarAction : null
		showContent: currentSectionPage ? currentSectionPage.showTitleBarContent : true
		showSeparator: currentSectionPage ? currentSectionPage.contentIsScrolled : false
		smartEidUsed: currentSectionPage ? currentSectionPage.smartEidUsed : false
		title: currentSectionPage ? currentSectionPage.title : ""
	}

	Component.onCompleted: {
		Style.dimens.screenHeight = Qt.binding(function () {
			return root.height;
		});
		feedback.showIfNecessary();
	}
	onClosing: pClose => {
		// back button pressed
		pClose.accepted = false;
		if (contentArea.visibleItem) {
			if (contentArea.activeModule === UiModule.DEFAULT) {
				let currentTime = new Date();
				if (currentTime - d.lastCloseInvocation < 1000) {
					UiPluginModel.fireQuitApplicationRequest();
					pClose.accepted = true;
					return;
				}
				d.lastCloseInvocation = currentTime;
				//: INFO ANDROID IOS Hint that is shown if the users pressed the "back" button on the top-most navigation level for the first time (a second press closes the app).
				ApplicationModel.showFeedback(qsTr("To close the app, press the back button 2 times."));
				return;
			}
			let activeStackView = contentArea.visibleItem;
			let navigationAction = contentArea.currentSectionPage.navigationAction;
			if (activeStackView.depth <= 1 && (!navigationAction || navigationAction.action !== NavigationAction.Action.Cancel) && contentArea.activeModule !== UiModule.ONBOARDING) {
				navigation.show(UiModule.DEFAULT);
			} else if (navigationAction && navigationAction.action !== NavigationAction.Action.None) {
				navigationAction.clicked(undefined);
			}
		}
	}

	QtObject {
		id: d

		property date lastCloseInvocation: new Date(0)
	}
	Action {
		enabled: UiPluginModel.debugBuild
		shortcut: "Escape"

		onTriggered: root.close()
	}
	Connections {
		function onFireApplicationActivated() {
			feedback.showIfNecessary();
		}

		target: UiPluginModel
	}
	Connections {
		function onFireShowRequest(pModule) {
			switch (pModule) {
			case UiModule.CURRENT:
				break;
			case UiModule.UPDATEINFORMATION:
				navigation.show(UiModule.DEFAULT);
				break;
			case UiModule.ONBOARDING:
				navigation.show(UiModule.HELP);
				break;
			case UiModule.IDENTIFY:
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE) {
					navigation.show(UiModule.SELF_AUTHENTICATION);
					break;
				}
			default:
				navigation.show(pModule);
				break;
			}
		}

		target: UiPluginModel
	}
	ContentArea {
		id: contentArea

		function reset() {
			currentSectionPage.popAll();
		}

		activeModule: navigation.activeModule

		anchors {
			bottomMargin: (navigation.lockedAndHidden ? UiPluginModel.safeAreaMargins.bottom : 0) + progressBar.visibleHeight
			fill: parent
			leftMargin: UiPluginModel.safeAreaMargins.left
			rightMargin: UiPluginModel.safeAreaMargins.right
		}
	}
	GStagedProgressBar {
		id: progressBar

		readonly property var currentSectionPage: contentArea.currentSectionPage
		readonly property real visibleHeight: visible ? height : 0

		progress: currentSectionPage ? currentSectionPage.progress : null
		visible: progress !== null && progress.enabled

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: UiPluginModel.safeAreaMargins.left
			right: parent.right
			rightMargin: UiPluginModel.safeAreaMargins.right
		}
	}
	IosBackGestureMouseArea {
		anchors.fill: contentArea
		visible: Style.is_layout_ios && titleBar.isBackAction

		onBackGestureTriggered: titleBar.navigationAction.clicked()
	}
	Connections {
		function onFireFeedbackChanged() {
			if (root.feedbackPopup) {
				root.feedbackPopup.close();
				root.feedbackPopup.destroy();
				root.feedbackPopup = null;
			}
			if (ApplicationModel.feedback !== "") {
				root.feedbackPopup = toast.createObject(root, {
					text: ApplicationModel.feedback
				});
				root.feedbackPopup.open();
			}
		}

		target: ApplicationModel
	}
	Component {
		id: toast

		ConfirmationPopup {
			closePolicy: ApplicationModel.isScreenReaderRunning ? Popup.NoAutoClose : Popup.CloseOnReleaseOutside
			dim: true
			modal: ApplicationModel.isScreenReaderRunning
			style: ApplicationModel.isScreenReaderRunning ? ConfirmationPopup.PopupStyle.OkButton : ConfirmationPopup.PopupStyle.NoButtons

			onConfirmed: ApplicationModel.onShowNextFeedback()
		}
	}
	StoreFeedbackPopup {
		id: feedback

		function showIfNecessary() {
			if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE && !RemoteServiceModel.running && SettingsModel.requestStoreFeedback()) {
				SettingsModel.hideFutureStoreFeedbackDialogs();
				feedback.open();
			}
		}
	}
}
