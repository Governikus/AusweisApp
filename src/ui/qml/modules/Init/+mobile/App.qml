/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

	function closeFeedbackPopup() {
		if (feedbackPopup) {
			feedbackPopup.close();
			feedbackPopup.destroy();
			feedbackPopup = null;
		}
	}
	function closeOpenPopups() {
		closeFeedbackPopup();
		feedback.close();
	}

	color: Style.color.background
	flags: Qt.platform.os === "ios" ? Qt.Window | Qt.ExpandedClientAreaHint : Qt.Window
	locale: Qt.locale(SettingsModel.language)
	visible: true

	footer: Navigation {
		id: navigation

		safeAreaBottomMargin: parent.SafeArea.margins.bottom

		onResetContentArea: contentArea.reset()

		anchors {
			left: parent.left
			leftMargin: parent.SafeArea.margins.left
			right: parent.right
			rightMargin: parent.SafeArea.margins.right
		}
	}
	menuBar: TitleBar {
		id: titleBar

		readonly property var currentSectionPage: contentArea.currentSectionPage
		readonly property bool isBackAction: navigationAction && navigationAction.action === NavigationAction.Action.Back

		enableTileStyle: currentSectionPage ? currentSectionPage.enableTileStyle : false
		navigationAction: currentSectionPage ? currentSectionPage.navigationAction : null
		rightAction: currentSectionPage ? currentSectionPage.rightTitleBarAction : null
		safeAreaTopMargin: parent.SafeArea.margins.top
		showContent: currentSectionPage ? currentSectionPage.showTitleBarContent : true
		showSeparator: currentSectionPage ? currentSectionPage.contentIsScrolled : false
		title: currentSectionPage ? currentSectionPage.title : ""

		anchors {
			left: parent.left
			leftMargin: parent.SafeArea.margins.left
			right: parent.right
			rightMargin: parent.SafeArea.margins.right
		}
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
			if (contentArea.activeModule === UiModule.DEFAULT && Qt.platform.os === "android") {
				let currentTime = new Date();
				if (currentTime - d.lastCloseInvocation < 1000) {
					UiPluginModel.fireQuitApplicationRequest();
					pClose.accepted = true;
					return;
				}
				d.lastCloseInvocation = currentTime;
				//: MOBILE Hint that is shown if the users pressed the "back" button on the top-most navigation level for the first time (a second press closes the app).
				ApplicationModel.showFeedback(qsTr("To close the app, tap the back button 2 times."));
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
			switch (ApplicationModel.currentWorkflow) {
			case ApplicationModel.Workflow.CHANGE_PIN:
				if (pModule !== UiModule.PINMANAGEMENT) {
					return;
				}
				break;
			case ApplicationModel.Workflow.SELF_AUTHENTICATION:
			case ApplicationModel.Workflow.AUTHENTICATION:
				if (pModule !== UiModule.IDENTIFY) {
					return;
				}
				break;
			case ApplicationModel.Workflow.REMOTE_SERVICE:
				if (pModule !== UiModule.REMOTE_SERVICE) {
					return;
				}
				break;
			case ApplicationModel.Workflow.NONE:
				break;
			}

			switch (pModule) {
			case UiModule.CURRENT:
				break;
			case UiModule.ONBOARDING:
				navigation.show(UiModule.HELP);
				break;
			case UiModule.IDENTIFY:
				root.closeOpenPopups();
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE) {
					navigation.show(UiModule.SELF_AUTHENTICATION);
					break;
				}
			// fall through
			default:
				navigation.show(pModule);
				break;
			}
		}

		target: UiPluginModel
	}
	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		ContentArea {
			id: contentArea

			function reset() {
				currentSectionPage.popAll();
			}

			Layout.fillHeight: true
			Layout.fillWidth: true
			activeModule: navigation.activeModule

			IosBackGestureMouseArea {
				anchors.fill: parent
				enabled: Qt.platform.os === "ios" && titleBar.isBackAction

				onBackGestureTriggered: titleBar.navigationAction.clicked()
			}
		}
		GStagedProgressBar {
			readonly property var currentSectionPage: contentArea.currentSectionPage

			Layout.fillWidth: true
			progress: currentSectionPage ? currentSectionPage.progress : null
			visible: progress !== null && progress.enabled
		}
	}
	ScreenshotPreventer {
		readonly property var currentSectionPage: contentArea.currentSectionPage

		preventScreenshots: currentSectionPage ? currentSectionPage.preventScreenshots : false

		onCurrentSectionPageChanged: Qt.callLater(notifyScreenRecording)
	}
	Connections {
		function onFireA11yFocusChanged(pItem) {
			Utils.positionViewAtItem(pItem);
		}
		function onFireFeedbackChanged() {
			root.closeFeedbackPopup();
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
			closePolicy: ApplicationModel.screenReaderRunning ? Popup.NoAutoClose : Popup.CloseOnReleaseOutside
			dim: true
			modal: ApplicationModel.screenReaderRunning
			style: ApplicationModel.screenReaderRunning ? ConfirmationPopup.PopupStyle.OkButton : ConfirmationPopup.PopupStyle.NoButtons

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
