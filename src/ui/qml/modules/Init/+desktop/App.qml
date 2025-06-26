/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import Governikus.FeedbackView
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

import QtQml
import QtQuick
import QtQuick.Controls

ApplicationWindow {
	id: root

	function showDetachedLogView() {
		if (d.detachedLogView === null) {
			d.detachedLogView = detachedLogViewWindow.createObject();
		} else {
			d.detachedLogView.raise();
		}
	}

	color: Style.color.background
	minimumHeight: 360
	minimumWidth: 480
	title: titleBar.title
	visible: false

	menuBar: TitleBar {
		id: titleBar

		Keys.forwardTo: [contentItem]
		contentItem: (contentArea.currentItem as SectionPage)
		showPane: contentArea.depth > 1

		onStartClicked: contentArea.pop(null)
	}

	Component.onCompleted: titleBar.forceActiveFocus(Qt.MouseFocusReason)
	onActiveChanged: {
		if (root.active && contentArea.activeModule === UiModule.DEFAULT) {
			switch (root.visibility) {
			case ApplicationWindow.Windowed:
			case ApplicationWindow.Maximized:
			case ApplicationWindow.FullScreen:
				UiPluginModel.showUpdateInformationIfPending();
			default:
				break;
			}
		}
	}
	onClosing: close => {
		ApplicationModel.fireAppAboutToQuit();
		close.accepted = closeHandler.handle();
	}
	onHeightChanged: d.setScaleFactor()
	onVisibilityChanged: visibility => {
		if (visibility !== ApplicationWindow.Minimized)
			d.lastVisibility = visibility;
	}
	onWidthChanged: d.setScaleFactor()

	Shortcut {
		enabled: Qt.platform.os === "osx"
		sequence: "Ctrl+W"

		onActivated: root.close()
	}
	Image {
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		height: 0.8 * parent.height
		opacity: 0.2
		source: "qrc:///images/logo_beta_testing.svg"
		visible: SettingsModel.showBetaTesting
		width: 0.8 * parent.width
	}
	Item {
		Component.onCompleted: {
			Style.software_renderer = GraphicsInfo.api === GraphicsInfo.Software;
		}
	}
	QtObject {
		id: d

		property ApplicationWindow detachedLogView: null
		property var dominationPopup: null
		readonly property string hideToTrayText: {
			if (Qt.platform.os === "osx") {
				//: INFO DESKTOP Content of the popup that is shown when the AA is closed and the close/minimize info was not disabled. macOS specific if autostart is enabled.
				return qsTr("The program remains available via the icon in the menu bar. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name);
			}

			//: INFO DESKTOP Content of the popup that is shown when the AA is closed and the close/minimize info was not disabled.
			return qsTr("The program remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name);
		}
		property int lastVisibility: ApplicationWindow.Windowed
		property bool suppressAbortWarning: false

		function abortCurrentWorkflow() {
			if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.AUTHENTICATION) {
				AuthModel.cancelWorkflow();
			} else if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
				SelfAuthModel.cancelWorkflow();
			} else if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.CHANGE_PIN) {
				ChangePinModel.cancelWorkflow();
			}
		}
		function ensureHeight(pMargin) {
			let preferredScreenHeight = root.Screen.height - 2 * pMargin;
			if (root.height > preferredScreenHeight) {
				root.height = preferredScreenHeight;
			}
		}
		function ensureScreenFit() {
			let windowMargin = 50;
			let screenMinX = root.Screen.virtualX;
			if (root.x < screenMinX) {
				ensureWidth(windowMargin);
				root.x = screenMinX + windowMargin;
			}
			let screenMinY = root.Screen.virtualY; // codespell:ignore
			if (root.y < screenMinY) {
				ensureHeight(windowMargin);
				root.y = screenMinY + windowMargin;
			}
			let screenMaxX = screenMinX + root.Screen.width;
			if (root.x + root.width > screenMaxX) {
				ensureWidth(windowMargin);
				root.x = screenMaxX - root.width - windowMargin;
			}
			let screenMaxY = screenMinY + root.Screen.height;
			if (root.y + root.height > screenMaxY) {
				ensureHeight(windowMargin);
				root.y = screenMaxY - root.height - windowMargin;
			}
		}
		function ensureWidth(pMargin) {
			let preferredScreenWidth = root.Screen.width - 2 * pMargin;
			if (root.width > preferredScreenWidth) {
				root.width = preferredScreenWidth;
			}
		}
		function hideUiAndTaskbarEntry() {
			root.hide();
			UiPluginModel.hideFromTaskbar();
		}
		function setScaleFactor() {
			let initialSize = UiPluginModel.initialWindowSize;
			UiPluginModel.scaleFactor = Math.min(root.width / initialSize.width, root.height / initialSize.height);
		}
		function showDetachedLogViewIfPresent() {
			if (d.detachedLogView !== null) {
				d.detachedLogView.show();
			}
		}
		function showMainWindow() {
			d.suppressAbortWarning = false;
			if (root.active) {
				return;
			}
			let currentFlags = root.flags;
			// Force the window to the foreground if it was minimized or is behind other windows (not closed to tray)
			if (Qt.platform.os === "windows") {
				root.flags = currentFlags | Qt.WindowStaysOnTopHint | Qt.WindowTitleHint;
			}
			if (d.lastVisibility === ApplicationWindow.Maximized) {
				root.showMaximized();
			} else {
				root.show();
			}
			root.flags = currentFlags;
			root.raise();
			root.requestActivate();
		}
	}
	ContentArea {
		id: contentArea

		anchors {
			bottomMargin: progressbar.height
			fill: parent
		}
	}
	GStagedProgressBar {
		id: progressbar

		readonly property var currentSectionPage: contentArea.currentItem

		activeFocusOnTab: ApplicationModel.isScreenReaderRunning
		progress: currentSectionPage ? currentSectionPage.progress : null
		visible: progress !== null && progress.enabled

		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
		}
	}
	DeveloperModeWarning {
		anchors.fill: parent
		visible: SettingsModel.developerMode && contentArea.activeModule !== UiModule.SETTINGS
	}
	DominationHandler {
	}
	CloseHandler {
		id: closeHandler

		hasOpenSubwindows: d.detachedLogView !== null

		onAbortWorkflow: d.abortCurrentWorkflow()
		onBringToFront: {
			root.show();
			root.raise();
		}
		onHide: {
			if (redirectOnSuccess) {
				AuthModel.continueWorkflow();
			}
			d.hideUiAndTaskbarEntry();
		}
		onQuit: {
			if (redirectOnSuccess) {
				AuthModel.continueWorkflow();
			}
			UiPluginModel.fireQuitApplicationRequest();
		}
		onShowMinimized: root.showMinimized()
	}
	Connections {
		function onFireHideRequest() {
			if (SettingsModel.trayIconEnabled) {
				d.hideUiAndTaskbarEntry();
				return;
			}
			if (Qt.platform.os === "osx") {
				root.hide();
			} else {
				root.showMinimized();
			}
		}
		function onFireProxyAuthenticationRequired(pProxyCredentials) {
			proxyCredentials.credentials = pProxyCredentials;
			proxyCredentials.open();
		}
		function onFireShowRequest(pModule) {
			d.showMainWindow();
			closeHandler.closeOpenDialogs();
			d.showDetachedLogViewIfPresent();
			contentArea.setUiModule(pModule);
			d.ensureScreenFit();
		}
		function onIsUpdatePendingChanged() {
			if (UiPluginModel.isUpdatePending && contentArea.activeModule === UiModule.DEFAULT) {
				UiPluginModel.showUpdateInformationIfPending();
			}
		}

		target: UiPluginModel
	}
	Connections {
		function onFireAppUpdateDataChanged(pAfterManualRequest) {
			if (!SettingsModel.appUpdateData.valid) {
				//: INFO DESKTOP Message that the update data is invalid and can't be used.
				ApplicationModel.showFeedback(qsTr("Failed to retrieve update information."));
			} else if (SettingsModel.appUpdateData.updateAvailable) {
				//: INFO DESKTOP An update was found which matches the current platform, the new version number is shown in the message.
				ApplicationModel.showFeedback(qsTr("An update is available (version %1).").arg(SettingsModel.appUpdateData.version));
			}
		}

		target: SettingsModel
	}
	ProxyCredentialsPopup {
		id: proxyCredentials

	}
	Component {
		id: detachedLogViewWindow

		DetachedLogViewWindow {
			onClosing: {
				d.detachedLogView.destroy();
				d.detachedLogView = null;
			}
		}
	}
}
