/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

Item {
	id: root

	//: INFO DESKTOP %1 is replaced with the application name
	readonly property string closeWarningText: qsTr("If the %1 is closed, it is no longer available for authentication. You must then restart the app to authenticate yourself to service providers.").arg(Qt.application.name)
	property var closingPopup: SettingsModel.trayIconEnabled ? closeWithEnabledTrayIconWarning : closeWarningDisabledTrayIcon
	readonly property string dontHideToTrayText: {
		if (Qt.platform.os === "osx") {
			return closeWarningText + "<br><br>" +
			//: INFO DESKTOP Content of the MacOS-popup that is shown when the AA is closed and the tray icon is disabled.
			qsTr("If you only close the user interface, the app remains active in the background in the future and can be opened again via the %1 icon on the menu bar.").arg(Qt.application.name);
		} else if (Qt.platform.os === "windows") {
			//: INFO DESKTOP Content of the Windows-popup that is shown when the AA is closed and the tray icon is disabled.
			return qsTr("If the %1 is terminated, it is no longer available for authentication. You must then restart the app in order to identify yourself to service providers.").arg(Qt.application.name) + "<br><br>" +
			//: INFO DESKTOP Content of the Windows-popup that is shown when the AA is closed and the tray icon is disabled.
			qsTr("If you only close the user interface, the app remains active in the background in the future and can be reopened via the %1 icon in the notification area of the Windows taskbar.").arg(Qt.application.name);
		}

		//: INFO DESKTOP Content of the popup that is shown when the AA is closed and the tray icon is disabled.
		return root.closeWarningText;
	}
	property bool hasOpenSubwindows: false
	readonly property string hideToTrayText: {
		if (Qt.platform.os === "osx") {
			//: INFO DESKTOP Content of the MacOS-popup that is shown when the AA is closed and the tray icon is enabled.
			return qsTr("The app remains active in the background and can be reopened via the %1 icon on the menu bar again.").arg(Qt.application.name) + "<br><br>" + closeWarningText;
		} else if (Qt.platform.os === "windows") {
			//: INFO DESKTOP Content of the Windows-popup that is shown when the AA is closed and the tray icon is enabled.
			return qsTr("The app remains active in the background and can be reopened via the %1 icon in the notification area of the Windows taskbar.").arg(Qt.application.name) + "<br><br>" + closeWarningText;
		}

		//: INFO DESKTOP Content of the popup that is shown when the AA is closed and the tray icon is enabled.
		return qsTr("The app remains available via the icon in the system tray. Click on the %1 icon to reopen the user interface.").arg(Qt.application.name);
	}

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
		if (ApplicationModel.currentWorkflow !== ApplicationModel.Workflow.NONE) {
			abortWorkflowWarning.open();
			bringToFront();
			return false;
		}
		if (hasOpenSubwindows) {
			showMinimized();
			return false;
		}
		if (SettingsModel.remindUserToClose) {
			closingPopup.open();
			bringToFront();
			return false;
		}
		closeOrHide();
		return true;
	}
	function handleFullClose() {
		SettingsModel.trayIconEnabled = false;
		root.quit();
	}
	function handleJustCloseUi() {
		SettingsModel.trayIconEnabled = true;
		root.closeOrHide();
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
	BaseConfirmationPopup {
		id: closeWithEnabledTrayIconWarning

		closePolicy: Popup.NoAutoClose
		showCloseButton: true
		text: root.hideToTrayText
		//: INFO DESKTOP Header of the popup that is shown when the AA is closed for the first time.
		title: qsTr("The user interface of the %1 is closed.").arg(Qt.application.name)

		buttons: RowLayout {
			layoutDirection: Qt.RightToLeft
			spacing: Style.dimens.pane_spacing
			width: parent.width

			GButton {
				style: Style.color.controlOptional
				//: LABEL DESKTOP
				text: qsTr("Completely close the app")

				onClicked: root.handleFullClose()
			}
			GButton {
				//: LABEL DESKTOP
				text: qsTr("Just close the user interface")

				onClicked: root.handleJustCloseUi()
			}
		}

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			//: LABEL DESKTOP
			text: qsTr("Do not display this message in future.")

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
	BaseConfirmationPopup {
		id: closeWarningDisabledTrayIcon

		closePolicy: Popup.NoAutoClose
		showCloseButton: true
		style: ConfirmationPopup.PopupStyle.OkButton
		text: root.dontHideToTrayText
		//: INFO DESKTOP Header of the popup that is shown when the AA is quit for the first time.
		title: qsTr("The %1 is closed.").arg(Qt.application.name)

		buttons: RowLayout {
			layoutDirection: Qt.RightToLeft
			spacing: Style.dimens.pane_spacing
			width: parent.width

			GButton {
				style: Style.color.controlOptional
				//: LABEL DESKTOP
				text: Qt.platform.os === "osx" ? qsTr("Close user interface to menu bar") : qsTr("Just close the user interface")

				onClicked: root.handleJustCloseUi()
			}
			GButton {
				//: LABEL DESKTOP
				text: qsTr("Completely close the app")

				onClicked: root.handleFullClose()
			}
		}

		GCheckBox {
			checked: !SettingsModel.remindUserToClose
			//: LABEL DESKTOP
			text: qsTr("Do not display this message in future.")

			onCheckedChanged: SettingsModel.remindUserToClose = !checked
		}
	}
}
