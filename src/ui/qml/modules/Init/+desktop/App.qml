/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import Governikus.Global
import Governikus.TitleBar
import Governikus.FeedbackView
import Governikus.MainView
import Governikus.SelfAuthenticationView
import Governikus.AuthView
import Governikus.ChangePinView
import Governikus.ProgressView
import Governikus.MoreView
import Governikus.SettingsView
import Governikus.SetupAssistantView
import Governikus.UpdateView
import Governikus.View
import Governikus.Type
import Governikus.Style
import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Controls

ApplicationWindow {
	id: appWindow

	property var dominationPopup: null

	function closingPopup() {
		if (SettingsModel.showTrayIcon) {
			return closeWarning;
		}
		return quitWarning;
	}
	function handleClosing() {
		if (SettingsModel.showTrayIcon) {
			d.hideUiAndTaskbarEntry();
			return;
		}
		UiPluginModel.fireQuitApplicationRequest();
	}
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
	title: menuBar.title
	visible: false

	menuBar: TitleBar {
		contentRoot: contentLoader

		onRootClicked: d.activeView = UiModule.DEFAULT
	}

	Component.onCompleted: menuBar.forceActiveFocus(Qt.MouseFocusReason)
	onClosing: close => {
		if (ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.NONE && !d.suppressAbortWarning) {
			abortWorkflowWarning.open();
			close.accepted = false;
			return;
		}
		if (d.detachedLogView !== null) {
			showMinimized();
			close.accepted = false;
			return;
		}
		if (SettingsModel.remindUserToClose && !d.suppressAbortWarning) {
			closingPopup().open();
			close.accepted = false;
		} else {
			handleClosing();
		}
	}
	onHeightChanged: d.setScaleFactor()
	onVisibilityChanged: visibility => {
		if (visibility !== ApplicationWindow.Minimized)
			d.lastVisibility = visibility;
	}
	onWidthChanged: d.setScaleFactor()

	Item {
		Component.onCompleted: {
			Style.software_renderer = GraphicsInfo.api === GraphicsInfo.Software;
		}
	}
	QtObject {
		id: d

		property int activeView: UiModule.DEFAULT
		property ApplicationWindow detachedLogView: null
		readonly property string hideToTrayText: {
			if (Qt.platform.os === "osx") {
				//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed and the close/minimize info was not disabled. macOS specific if autostart is enabled.
				return qsTr("The program remains available via the icon in the menu bar. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name);
			}

			//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed and the close/minimize info was not disabled.
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
		function closeOpenDialogs() {
			closeWarning.close();
			quitWarning.close();
			abortWorkflowWarning.close();
		}
		function ensureHeight(pMargin) {
			let preferredScreenHeight = appWindow.screen.height - 2 * pMargin;
			if (appWindow.height > preferredScreenHeight) {
				appWindow.height = preferredScreenHeight;
			}
		}
		function ensureScreenFit() {
			let windowMargin = 50;
			let screenMinX = appWindow.screen.virtualX;
			if (appWindow.x < screenMinX) {
				ensureWidth(windowMargin);
				appWindow.x = screenMinX + windowMargin;
			}
			let screenMinY = appWindow.screen.virtualY;
			if (appWindow.y < screenMinY) {
				ensureHeight(windowMargin);
				appWindow.y = screenMinY + windowMargin;
			}
			let screenMaxX = screenMinX + appWindow.screen.width;
			if (appWindow.x + appWindow.width > screenMaxX) {
				ensureWidth(windowMargin);
				appWindow.x = screenMaxX - appWindow.width - windowMargin;
			}
			let screenMaxY = screenMinY + appWindow.screen.height;
			if (appWindow.y + appWindow.height > screenMaxY) {
				ensureHeight(windowMargin);
				appWindow.y = screenMaxY - appWindow.height - windowMargin;
			}
		}
		function ensureWidth(pMargin) {
			let preferredScreenWidth = appWindow.screen.width - 2 * pMargin;
			if (appWindow.width > preferredScreenWidth) {
				appWindow.width = preferredScreenWidth;
			}
		}
		function hideUiAndTaskbarEntry() {
			hide();
			UiPluginModel.hideFromTaskbar();
		}
		function setScaleFactor() {
			let initialSize = UiPluginModel.initialWindowSize;
			UiPluginModel.scaleFactor = Math.min(width / initialSize.width, height / initialSize.height);
		}
		function showDetachedLogViewIfPresent() {
			if (d.detachedLogView !== null) {
				d.detachedLogView.show();
			}
		}
		function showMainWindow() {
			d.suppressAbortWarning = false;
			if (active) {
				return;
			}
			let currentFlags = flags;
			// Force the window to the foreground if it was minimized or is behind other windows (not closed to tray)
			if (Qt.platform.os === "windows") {
				flags = currentFlags | Qt.WindowStaysOnTopHint | Qt.WindowTitleHint;
			}
			if (d.lastVisibility === ApplicationWindow.Maximized) {
				showMaximized();
			} else {
				show();
			}
			flags = currentFlags;
			raise();
			requestActivate();
		}
	}
	Component {
		id: domination

		ConfirmationPopup {
			closePolicy: Popup.NoAutoClose
			mainTextFormat: Text.PlainText
			style: ConfirmationPopup.PopupStyle.NoButtons
			//: INFO DESKTOP The AA2 is currently remote controlled via the SDK interface, concurrent usage of the AA2 is not possible.
			title: qsTr("Another application uses %1").arg(Qt.application.name)
		}
	}
	ConfirmationPopup {
		id: closeWarning

		closePolicy: Popup.NoAutoClose
		style: ConfirmationPopup.PopupStyle.OkButton
		text: d.hideToTrayText
		//: INFO DESKTOP Header of the popup that is shown when the AA2 is closed for the first time.
		title: qsTr("The user interface of the %1 is closed.").arg(Qt.application.name)

		onConfirmed: handleClosing()

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			//: LABEL DESKTOP
			text: qsTr("Do not show this dialog again.")

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
	ConfirmationPopup {
		id: quitWarning

		closePolicy: Popup.NoAutoClose
		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO DESKTOP Text of the popup that is shown when the AA2 is quit for the first time.
		text: qsTr("The %1 will be shut down and an authentication will no longer be possible. You will have to restart the %1 to identify yourself towards providers.").arg(Qt.application.name)
		//: INFO DESKTOP Header of the popup that is shown when the AA2 is quit for the first time.
		title: qsTr("The %1 is closed.").arg(Qt.application.name)

		onConfirmed: handleClosing()

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			//: LABEL DESKTOP
			text: qsTr("Do not show this dialog again.")

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
	ConfirmationPopup {
		id: abortWorkflowWarning

		readonly property string abortText: {
			if (SettingsModel.showTrayIcon) {
				//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed and a workflow is still active.
				return qsTr("This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.").arg(Qt.application.name);
			}
			//: INFO DESKTOP Content of the popup that is shown when the AA2 is shut down and a workflow is still active.
			return qsTr("This will cancel the current operation and shut the %1 down. You will have to restart the %1 to restart the operation.").arg(Qt.application.name);
		}

		closePolicy: Popup.NoAutoClose
		text: "%1%2".arg(abortText).arg(SettingsModel.remindUserToClose && SettingsModel.showTrayIcon ? "<br/><br/>%1".arg(d.hideToTrayText) : "")
		//: INFO DESKTOP Header of the popup that is shown when the AA2 is closed and a workflow is still active
		title: qsTr("Abort operation")

		onCancelled: close()
		onConfirmed: {
			d.abortCurrentWorkflow();
			d.suppressAbortWarning = true;
			appWindow.close();
		}
	}
	Connections {
		function onFireDominatorChanged() {
			if (dominationPopup) {
				dominationPopup.close();
				dominationPopup.destroy();
				dominationPopup = null;
			}
			if (UiPluginModel.dominated) {
				dominationPopup = domination.createObject(appWindow, {
					"text": UiPluginModel.dominator
				});
				dominationPopup.open();
			}
		}
		function onFireHideRequest() {
			if (SettingsModel.showTrayIcon) {
				d.hideUiAndTaskbarEntry();
				return;
			}
			if (Qt.platform.os === "osx") {
				hide();
			} else {
				showMinimized();
			}
		}
		function onFireShowRequest(pModule) {
			d.showMainWindow();
			d.closeOpenDialogs();
			d.showDetachedLogViewIfPresent();
			switch (pModule) {
			case UiModule.CURRENT:
				break;
			case UiModule.IDENTIFY:
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE) {
					d.activeView = UiModule.SELF_AUTHENTICATION;
				}
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.AUTHENTICATION || ApplicationModel.currentWorkflow === ApplicationModel.Workflow.SELF_AUTHENTICATION) {
					d.activeView = UiModule.IDENTIFY;
				}
				break;
			case UiModule.PINMANAGEMENT:
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE || ApplicationModel.currentWorkflow === ApplicationModel.Workflow.CHANGE_PIN) {
					d.activeView = UiModule.PINMANAGEMENT;
				}
				break;
			case UiModule.UPDATEINFORMATION:
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE && d.activeView === UiModule.DEFAULT) {
					d.activeView = UiModule.UPDATEINFORMATION;
				}
				break;
			default:
				if (ApplicationModel.currentWorkflow === ApplicationModel.Workflow.NONE) {
					d.activeView = pModule;
				}
				break;
			}
			d.ensureScreenFit();
		}

		target: UiPluginModel
	}
	Connections {
		function onFireAppUpdateDataChanged() {
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
	Shortcut {
		enabled: Qt.platform.os === "osx"
		sequence: "Ctrl+W"

		onActivated: close()
	}
	Image {
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		height: 0.8 * parent.height
		opacity: 0.2
		source: "qrc:///images/logo_beta_testing.svg"
		visible: UiPluginModel.developerVersion && SettingsModel.showBetaTesting
		width: 0.8 * parent.width
	}
	Loader {
		id: contentLoader

		anchors.fill: parent
		sourceComponent: switch (d.activeView) {
		case UiModule.SELF_AUTHENTICATION:
			return selfauthentication;
		case UiModule.IDENTIFY:
			return auth;
		case UiModule.PINMANAGEMENT:
			return pinmanagement;
		case UiModule.HELP:
			return help;
		case UiModule.SETTINGS:
			return settings;
		case UiModule.TUTORIAL:
			if (SettingsModel.autoStartAvailable && !SettingsModel.autoStartSetByAdmin) {
				return tutorial;
			}
			return main;
		case UiModule.UPDATEINFORMATION:
			return updateinformation;
		default:
			return main;
		}

		Keys.onEscapePressed: {
			if (d.activeView !== UiModule.DEFAULT) {
				d.activeView = UiModule.DEFAULT;
			}
		}
		onItemChanged: {
			menuBar.updateActions();
			item.setActive();
		}

		Component {
			id: main

			MainView {
			}
		}
		Component {
			id: selfauthentication

			SelfAuthenticationView {
			}
		}
		Component {
			id: auth

			AuthView {
			}
		}
		Component {
			id: pinmanagement

			ChangePinView {
			}
		}
		Component {
			id: help

			MoreView {
			}
		}
		Component {
			id: settings

			SettingsView {
			}
		}
		Component {
			id: tutorial

			SetupAutostartView {
			}
		}
		Component {
			id: updateinformation

			UpdateView {
				onLeaveView: d.activeView = UiModule.DEFAULT
			}
		}
		Connections {
			function onNextView(pName) {
				d.activeView = pName;
			}
			function onVisibleChildrenChanged() {
				menuBar.updateActions();
			}

			target: contentLoader.item
		}
	}
	Rectangle {
		color: Style.color.paneSublevel.background.basic
		height: childrenRect.height
		opacity: 0.7
		radius: Style.dimens.pane_radius
		visible: SettingsModel.developerMode && d.activeView !== UiModule.SETTINGS
		width: childrenRect.width

		anchors {
			bottom: parent.bottom
			bottomMargin: 4
			horizontalCenter: parent.horizontalCenter
		}
		Row {
			padding: Constants.pane_padding / 2
			spacing: Constants.groupbox_spacing

			GText {
				anchors.verticalCenter: parent.verticalCenter

				//: LABEL DESKTOP
				text: qsTr("Developer Mode: Enabled!")
			}
			GButton {
				//: LABEL DESKTOP Global button to disable developer mode.
				text: qsTr("Disable")

				onClicked: SettingsModel.developerMode = false
			}
		}
	}
	Crossed {
		height: contentLoader.height
		visible: SettingsModel.developerMode && d.activeView !== UiModule.SETTINGS
		width: contentLoader.width
	}
	Connections {
		function onFireProxyAuthenticationRequired(pProxyCredentials) {
			proxyCredentials.credentials = pProxyCredentials;
			proxyCredentials.open();
		}

		target: UiPluginModel
	}
	ProxyCredentialsPopup {
		id: proxyCredentials

	}
	Component {
		id: detachedLogViewWindow

		ApplicationWindow {
			height: UiPluginModel.initialWindowSize.height
			minimumHeight: UiPluginModel.initialWindowSize.height
			minimumWidth: UiPluginModel.initialWindowSize.width
			title: qsTr("Detached log viewer")
			visible: true
			width: UiPluginModel.initialWindowSize.width

			onClosing: {
				d.detachedLogView.destroy();
				d.detachedLogView = null;
			}

			Shortcut {
				enabled: Qt.platform.os === "osx"
				sequence: "Ctrl+W"

				onActivated: close()
			}
			DetachedLogView {
				anchors.fill: parent
				focus: true
			}
		}
	}
}
