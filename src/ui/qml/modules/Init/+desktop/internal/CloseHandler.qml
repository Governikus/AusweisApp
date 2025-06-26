/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Global
import Governikus.Type

Item {
	id: root

	//: LABEL DESKTOP
	readonly property string buttonCloseText: qsTr("Close completely")
	//: LABEL DESKTOP
	readonly property string buttonLeaveActiveText: qsTr("Leave active in the background")
	//: INFO DESKTOP %1 is replaced with the application name
	readonly property string closeWarningText: qsTr("If the %1 is closed, it is no longer available for authentication. You must then restart the app to authenticate yourself to service providers.").arg(Qt.application.name)
	//: INFO DESKTOP Header of the popup that is shown when the AA is closed for the first time.
	readonly property string closeWarningTitle: qsTr("How should the %1 be closed in the future?").arg(Qt.application.name)
	property var closingPopup: SettingsModel.trayIconEnabled ? closeWithEnabledTrayIconWarning : closeWarningDisabledTrayIcon
	readonly property string dontHideToTrayText: {
		return closeWarningText + "<br><br>" + remainActiveWarningText + "<br><br>" + settingsNoteText;
	}
	property bool hasOpenSubwindows: false
	readonly property string hideToTrayText: {
		return remainActiveWarningText + "<br><br>" + closeWarningText + "<br><br>" + settingsNoteText;
	}
	readonly property bool redirectOnSuccess: AuthModel.statusCode === GlobalStatusCode.No_Error && AuthModel.currentState === "StateRedirectBrowser"
	//: INFO DESKTOP %1 is replaced with the application name
	readonly property string remainActiveWarningText: qsTr("If the %1 remains active in the background, it will open automatically as soon as you start an authentication. You can still open the %1 manually at any time.").arg(Qt.application.name)
	//: INFO DESKTOP Note to the user that the setting is available in the settings
	readonly property string settingsNoteText: qsTr("You can change your selection at any time in the settings.")
	//: LABEL DESKTOP
	readonly property string settingsReminderText: qsTr("Do not display this message in future.")

	signal abortWorkflow
	signal bringToFront
	signal hide
	signal quit
	signal showMinimized

	function closeOpenDialogs() {
		closeWithEnabledTrayIconWarning.close();
		closeWarningDisabledTrayIcon.close();
		abortWorkflowWarning.close();
	}
	function closeOrHide() {
		if (SettingsModel.trayIconEnabled) {
			hide();
			return;
		}
		quit();
	}
	function handle() {
		if (redirectOnSuccess) {
			return showPopupOrCloseHide();
		}
		if (ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.NONE) {
			abortWorkflowWarning.open();
			bringToFront();
			return false;
		}
		if (hasOpenSubwindows) {
			showMinimized();
			return false;
		}
		return showPopupOrCloseHide();
	}
	function handleFullClose() {
		SettingsModel.trayIconEnabled = false;
		root.quit();
	}
	function handleJustCloseUi() {
		SettingsModel.trayIconEnabled = true;
		root.closeOrHide();
	}
	function showPopupOrCloseHide() {
		if (SettingsModel.remindUserToClose) {
			closingPopup.open();
			bringToFront();
			return false;
		}
		closeOrHide();
		return true;
	}

	ConfirmationPopup {
		id: abortWorkflowWarning

		readonly property string abortText: {
			if (SettingsModel.trayIconEnabled) {
				//: INFO DESKTOP Content of the popup that is shown when the AA is closed and a workflow is still active.
				return qsTr("This will cancel the current operation and hide the UI of %1. You can restart the operation at any time.").arg(Qt.application.name);
			}
			//: INFO DESKTOP Content of the popup that is shown when the AA is shut down and a workflow is still active.
			return qsTr("This will cancel the current operation and shut the %1 down. You will have to restart the %1 to restart the operation.").arg(Qt.application.name);
		}

		closePolicy: Popup.NoAutoClose
		//: INFO DESKTOP
		okButtonText: qsTr("Abort process")
		text: abortText
		//: INFO DESKTOP Header of the popup that is shown when the AA is closed and a workflow is still active
		title: qsTr("Abort operation")

		onCancelled: close()
		onConfirmed: {
			root.abortWorkflow();
			if (!UiPluginModel.isUpdatePending || ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.AUTHENTICATION) {
				root.closeOrHide();
			}
			close();
		}
	}
	ConfirmationPopup {
		id: closeWithEnabledTrayIconWarning

		cancelButtonText: root.buttonCloseText
		closePolicy: Popup.NoAutoClose
		okButtonText: root.buttonLeaveActiveText
		showCloseButton: true
		text: root.hideToTrayText
		title: root.closeWarningTitle

		onCancelled: root.handleFullClose()
		onConfirmed: root.handleJustCloseUi()

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			horizontalPadding: 0
			text: root.settingsReminderText

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
	ConfirmationPopup {
		id: closeWarningDisabledTrayIcon

		cancelButtonText: root.buttonLeaveActiveText
		closePolicy: Popup.NoAutoClose
		okButtonText: root.buttonCloseText
		showCloseButton: true
		text: root.dontHideToTrayText
		title: root.closeWarningTitle

		onCancelled: root.handleJustCloseUi()
		onConfirmed: root.handleFullClose()

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			horizontalPadding: 0
			text: root.settingsReminderText

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
}
