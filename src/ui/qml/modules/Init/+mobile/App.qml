/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.TitleBar
import Governikus.Navigation
import Governikus.View
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
	function showAppRatingIfNecessary() {
		if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE && !RemoteServiceModel.running) {
			ApplicationModel.showAppStoreRatingDialog();
		}
	}

	color: Style.color.background
	flags: Qt.Window | Qt.ExpandedClientAreaHint
	locale: Qt.locale(SettingsModel.language)
	visible: true

	footer: Navigation {
		id: navigation

		readonly property bool currentlyLockedAndHidden: contentArea.currentSectionPage?.lockAndHideNavigation ?? false
		readonly property bool onboardingActive: contentArea.activeModule === UiModule.ONBOARDING
		readonly property bool workflowActive: ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.NONE

		lockedAndHidden: workflowActive || onboardingActive || currentlyLockedAndHidden
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
		showAppRatingIfNecessary();
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

	palette {
		toolTipBase: Style.color.background
		toolTipText: Style.color.textNormal.basic_unchecked
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
			root.showAppRatingIfNecessary();
		}

		target: UiPluginModel
	}
	Connections {
		function onFireShowRequest(pModule) {
			switch (ApplicationModel.currentWorkflow) {
			case ApplicationModel.Workflow.SELF_AUTHENTICATION:
			case ApplicationModel.Workflow.AUTHENTICATION:
				if (pModule === UiModule.IDENTIFY) {
					break;
				}
			// fallthrough
			case ApplicationModel.Workflow.CHANGE_PIN:
			case ApplicationModel.Workflow.REMOTE_SERVICE:
				if (navigation.lockedAndHidden) {
					console.log("Suppressing activation of UiModule", pModule, "since a workflow is active or the navigation is hidden");
					return;
				}
				break;
			default:
				break;
			}

			switch (pModule) {
			case UiModule.CURRENT:
				break;
			case UiModule.ONBOARDING:
				navigation.show(UiModule.HELP);
				break;
			case UiModule.IDENTIFY:
				root.closeFeedbackPopup();
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
				currentSectionPage?.popAll();
				root.showAppRatingIfNecessary();
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
	Connections {
		function onFireUpdateAvailable() {
			updateAvailablePopup.open();
		}
		function onFireUpdateCanceled() {
			updateCanceledPopup.open();
		}

		target: SettingsModel.appUpdateData
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
	ConfirmationPopup {
		id: updateAvailablePopup

		closePolicy: Popup.NoAutoClose
		dim: true
		modal: true
		//: MOBILE
		okButtonText: qsTr("Restart now")
		//: MOBILE
		text: qsTr("An update was downloaded and a restart is required to apply it.")
		//: MOBILE
		title: qsTr("Update available")

		onCancelled: close()
		onConfirmed: SettingsModel.appUpdateData.applyUpdate()
	}
	ConfirmationPopup {
		id: updateCanceledPopup

		readonly property bool missingNetworkAccess: !connectivityManager.networkInterfaceActive

		//: MOBILE
		cancelButtonText: qsTr("Exit")
		closePolicy: Popup.NoAutoClose
		dim: true
		modal: true
		//: MOBILE
		okButtonText: qsTr("Install update")
		style: ConfirmationPopup.PopupStyle.CancelButton | (missingNetworkAccess ? 0 : ConfirmationPopup.PopupStyle.OkButton)
		//: MOBILE
		title: qsTr("Update required")

		onCancelled: UiPluginModel.fireQuitApplicationRequest()
		onConfirmed: SettingsModel.appUpdateData.startUpdateFlow(true)

		ColumnLayout {
			spacing: Style.dimens.groupbox_spacing
			width: parent.width

			GText {
				//: MOBILE %1 is replaced with the application name
				text: qsTr("A critical update is available and required to use the %1.").arg(Qt.application.name)
			}
			GText {
				font.weight: Style.font.bold
				//: MOBILE
				text: qsTr("A network connection is required to install the update.")
				visible: updateCanceledPopup.missingNetworkAccess
			}
		}
	}
	ConnectivityManager {
		id: connectivityManager

		watching: updateCanceledPopup.visible
	}
}
