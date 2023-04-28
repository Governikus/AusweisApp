/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.FeedbackView 1.0
import Governikus.MainView 1.0
import Governikus.HistoryView 1.0
import Governikus.SelfAuthenticationView 1.0
import Governikus.AuthView 1.0
import Governikus.ChangePinView 1.0
import Governikus.ProgressView 1.0
import Governikus.ProviderView 1.0
import Governikus.MoreView 1.0
import Governikus.SettingsView 1.0
import Governikus.TutorialView 1.0
import Governikus.UpdateView 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.AuthModel 1.0
import Governikus.Type.UiModule 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Style 1.0
import QtQml 2.15
import QtQml.Models 2.15
import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
	id: appWindow
	function showDetachedLogView() {
		if (d.detachedLogView === null) {
			d.detachedLogView = detachedLogViewWindow.createObject(appWindow);
		} else {
			d.detachedLogView.raise();
		}
	}

	color: Style.color.background
	minimumHeight: 360
	minimumWidth: 480
	title: menuBar.rightMostAction.text
	visible: false

	menuBar: TitleBar {
		contentRoot: contentLoader

		onRootClicked: d.activeView = UiModule.DEFAULT
	}

	Component.onCompleted: menuBar.forceActiveFocus(Qt.MouseFocusReason)
	onClosing: close => {
		if (ApplicationModel.currentWorkflow !== ApplicationModel.WORKFLOW_NONE && !d.suppressAbortWarning) {
			abortWorkflowWarning.open();
			close.accepted = false;
			return;
		}
		if (d.isMacOsAndAutoStartDisabled) {
			close.accepted = true;
			return;
		}
		if (SettingsModel.remindUserToClose && !d.suppressAbortWarning) {
			closeWarning.open();
			close.accepted = false;
		} else {
			d.hideUiAndTaskbarEntry();
		}
	}
	onHeightChanged: d.setScaleFactor()
	onVisibilityChanged: visibility => {
		if (visibility !== ApplicationWindow.Minimized)
			d.lastVisibility = visibility;
	}
	onWidthChanged: d.setScaleFactor()

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
		readonly property bool isMacOsAndAutoStartDisabled: Qt.platform.os === "osx" && !SettingsModel.autoStartApp
		property int lastVisibility: ApplicationWindow.Windowed
		property bool suppressAbortWarning: false

		function abortCurrentWorkflow() {
			if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_AUTHENTICATION) {
				AuthModel.cancelWorkflow();
			} else if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_SELF_AUTHENTICATION) {
				SelfAuthModel.cancelWorkflow();
			} else if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN) {
				ChangePinModel.cancelWorkflow();
			}
		}
		function closeOpenDialogs() {
			closeWarning.close();
			abortWorkflowWarning.close();
		}
		function hideUiAndTaskbarEntry() {
			hide();
			plugin.hideFromTaskbar();
		}
		function setScaleFactor() {
			let initialSize = plugin.initialWindowSize;
			ApplicationModel.scaleFactor = Math.min(width / initialSize.width, height / initialSize.height);
		}
		function showMainWindow() {
			d.suppressAbortWarning = false;
			if (active) {
				return;
			}
			var currentFlags = flags;
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
	ConfirmationPopup {
		id: domination
		closePolicy: Popup.NoAutoClose
		style: ConfirmationPopup.PopupStyle.NoButtons
		text: plugin.dominator
		//: INFO DESKTOP The AA2 is currently remote controlled via the SDK interface, concurrent usage of the AA2 is not possible.
		title: qsTr("Another application uses %1").arg(Qt.application.name)
		visible: plugin.dominated
	}
	ConfirmationPopup {
		id: closeWarning
		closePolicy: Popup.NoAutoClose
		style: ConfirmationPopup.PopupStyle.OkButton
		text: d.hideToTrayText
		//: INFO DESKTOP Header of the popup that is shown when the AA2 is closed for the first time.
		title: qsTr("The user interface of the %1 is closed.").arg(Qt.application.name)

		onConfirmed: d.hideUiAndTaskbarEntry()

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			//: LABEL DESKTOP
			text: qsTr("Do not show this dialog again.")
			textStyle: Style.text.normal

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
	ConfirmationPopup {
		id: abortWorkflowWarning

		//: INFO DESKTOP Content of the popup that is shown when the AA2 is closed and a workflow is still active.
		readonly property string abortText: qsTr("This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.").arg(Qt.application.name)

		closePolicy: Popup.NoAutoClose
		text: "%1%2".arg(abortText).arg(SettingsModel.remindUserToClose && !d.isMacOsAndAutoStartDisabled ? "<br/><br/>%1".arg(d.hideToTrayText) : "")
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
		function onFireHideRequest() {
			hide();
		}
		function onFireShowRequest(pModule) {
			d.showMainWindow();
			d.closeOpenDialogs();
			switch (pModule) {
			case UiModule.CURRENT:
				break;
			case UiModule.IDENTIFY:
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE) {
					d.activeView = UiModule.SELF_AUTHENTICATION;
				}
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_AUTHENTICATION || ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_SELF_AUTHENTICATION) {
					d.activeView = UiModule.IDENTIFY;
				}
				break;
			case UiModule.PINMANAGEMENT:
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE || ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_CHANGE_PIN) {
					d.activeView = UiModule.PINMANAGEMENT;
				}
				break;
			case UiModule.UPDATEINFORMATION:
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE && d.activeView === UiModule.DEFAULT) {
					d.activeView = UiModule.UPDATEINFORMATION;
				}
				break;
			default:
				if (ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_NONE) {
					d.activeView = pModule;
				}
				break;
			}
		}

		target: plugin
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
		sequence: StandardKey.HelpContents

		onActivated: ApplicationModel.openOnlineHelp(menuBar.rightMostAction.helpTopic)
	}
	Image {
		anchors.centerIn: parent
		fillMode: Image.PreserveAspectFit
		height: 0.8 * parent.height
		opacity: 0.2
		source: "qrc:///images/logo_beta_testing.svg"
		visible: plugin.developerVersion && SettingsModel.showBetaTesting
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
		case UiModule.PROVIDER:
			return provider;
		case UiModule.HELP:
			return help;
		case UiModule.SETTINGS:
			return settings;
		case UiModule.HISTORY:
			return history;
		case UiModule.TUTORIAL:
			return tutorial;
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
			id: provider
			ProviderView {
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
			id: history
			HistoryView {
			}
		}
		Component {
			id: tutorial
			SetupAssistantView {
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
		color: Constants.white
		height: childrenRect.height
		opacity: 0.7
		radius: ApplicationModel.scaleFactor * 4
		visible: SettingsModel.developerMode && d.activeView !== UiModule.SETTINGS
		width: childrenRect.width

		anchors {
			bottom: parent.bottom
			bottomMargin: 4
			right: parent.right
			rightMargin: 4
		}
		Row {
			padding: Constants.pane_padding / 2
			spacing: Constants.groupbox_spacing

			GText {
				anchors.verticalCenter: parent.verticalCenter

				//: LABEL DESKTOP
				text: qsTr("Developer Mode: Enabled!")
				textStyle: Style.text.normal_warning
			}
			GButton {
				//: LABEL DESKTOP Global button to disable developer mode.
				text: qsTr("Disable")

				onClicked: SettingsModel.developerMode = false
			}
		}
	}
	Rectangle {
		id: developerWarning
		anchors.verticalCenter: parent.bottom
		antialiasing: true
		color: Constants.red
		height: ApplicationModel.scaleFactor * 50
		opacity: 0.5
		rotation: -Math.atan(contentLoader.height / contentLoader.width) * 180 / Math.PI
		transformOrigin: Item.Left
		visible: SettingsModel.developerMode && d.activeView !== UiModule.SETTINGS
		width: Math.sqrt(contentLoader.width * contentLoader.width + contentLoader.height * contentLoader.height)
	}
	Connections {
		function onFireProxyAuthenticationRequired(pProxyCredentials) {
			proxyCredentials.credentials = pProxyCredentials;
			proxyCredentials.open();
		}

		target: plugin
	}
	ProxyCredentialsPopup {
		id: proxyCredentials
	}
	Component {
		id: detachedLogViewWindow
		ApplicationWindow {
			height: plugin.initialWindowSize.height
			minimumHeight: appWindow.minimumHeight
			minimumWidth: appWindow.minimumWidth
			title: qsTr("Detached log viewer")
			visible: true
			width: plugin.initialWindowSize.width

			onClosing: {
				d.detachedLogView.destroy();
				d.detachedLogView = null;
			}

			DetachedLogView {
				anchors.fill: parent
				focus: true
			}
		}
	}
}
