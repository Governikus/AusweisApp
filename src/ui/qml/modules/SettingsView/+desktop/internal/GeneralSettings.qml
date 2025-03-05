/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	spacing: Style.dimens.pane_spacing

	GPane {
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing
		//: LABEL DESKTOP
		title: qsTr("Change language")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		LanguageButtons {
			columns: 4
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: 0
		//: LABEL DESKTOP
		title: qsTr("Appearance")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		DarkModeButtons {
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useSystemFont

			//: LABEL DESKTOP
			description: qsTr("Toggling will restart the %1").arg(Qt.application.name)
			drawBottomCorners: true
			//: LABEL DESKTOP
			text: qsTr("Use the system font")

			onCheckedChanged: {
				if (checked !== SettingsModel.useSystemFont) {
					SettingsModel.useSystemFont = checked;
					UiPluginModel.doRefresh();
				}
			}
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: 0
		//: LABEL DESKTOP
		title: qsTr("Accessibility")

		GSwitch {
			Layout.fillWidth: true
			checked: !SettingsModel.useAnimations
			//: LABEL DESKTOP
			text: qsTr("Use images instead of animations")

			onCheckedChanged: SettingsModel.useAnimations = !checked
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.visualPrivacy
			//: LABEL DESKTOP
			text: qsTr("Hide key animations when entering PIN")

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
		GSwitch {
			Layout.fillWidth: true
			checked: !SettingsModel.autoRedirectAfterAuthentication
			//: LABEL DESKTOP
			description: qsTr("After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.")
			drawBottomCorners: true
			//: LABEL DESKTOP
			text: qsTr("Manual redirection back to the provider")

			onCheckedChanged: SettingsModel.autoRedirectAfterAuthentication = !checked
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: 0
		//: LABEL DESKTOP
		title: qsTr("Behavior")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.autoStartApp
			//: LABEL DESKTOP Description for auto-start option
			description: qsTr("The %1 gets started on system boot, so that it can be opened automatically on an authentication. It has to be started manually otherwise.").arg(Qt.application.name)
			enabled: !SettingsModel.autoStartSetByAdmin && SettingsModel.autoStartAvailable
			//: LABEL DESKTOP Text for auto-start option
			text: qsTr("Automatically start %1 (recommended)").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoStartApp = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.trayIconEnabled
			//: LABEL MACOS Description for attaching the AA to the menu bar/system tray
			description: qsTr("The %1 continues to run in the background after the application window is closed, so that it can be opened automatically on an authentication.").arg(Qt.application.name)
			text: Qt.platform.os === "osx" ?
			//: LABEL MACOS Text for attaching the AA to the menu bar
			qsTr("Attach %1 to menu bar (recommended)").arg(Qt.application.name) :
			//: LABEL WINDOWS Text for attaching the AA to the system tray
			qsTr("Attach %1 to system tray (recommended)").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.trayIconEnabled = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.autoCloseWindowAfterAuthentication

			//: LABEL DESKTOP
			text: qsTr("Close %1 window after authentication").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.showInAppNotifications

			//: LABEL DESKTOP Only visible when the user activates the developer mode in the settings.
			description: SettingsModel.developerMode ? qsTr("Using the developer mode forces the notifications to be enabled.") : ""
			drawBottomCorners: true
			enabled: !SettingsModel.developerMode

			//: LABEL DESKTOP
			text: qsTr("Show notifications inside of %1").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.showInAppNotifications = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: Style.dimens.pane_spacing
		//: LABEL DESKTOP
		title: qsTr("Network")
		visible: SettingsModel.customProxyAttributesPresent

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useCustomProxy
			drawBottomCorners: true

			//: LABEL DESKTOP
			text: qsTr("Use the proxy (%1) specified during the installation.").arg(SettingsModel.customProxyUrl)

			onCheckedChanged: SettingsModel.useCustomProxy = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
}
