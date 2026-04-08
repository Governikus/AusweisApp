/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type
import Governikus.Style

ColumnLayout {
	id: root

	signal showUpdateRequested

	spacing: Style.dimens.pane_spacing

	GPane {
		Layout.fillWidth: true
		//: DESKTOP
		title: qsTr("Change language")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		LanguageButtons {
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		contentSpacing: Style.dimens.pane_spacing / 2
		//: DESKTOP
		title: qsTr("Appearance")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		DarkModeButtons {
			Layout.leftMargin: Style.dimens.pane_padding
			Layout.rightMargin: Style.dimens.pane_padding
		}
		GSwitch {
			checked: SettingsModel.useSystemFont

			//: DESKTOP
			description: qsTr("Toggling will restart the %1").arg(Qt.application.name)
			drawBottomCorners: true
			//: DESKTOP
			text: qsTr("Use system font")

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
		contentSpacing: 0
		//: DESKTOP
		title: qsTr("Accessibility")

		GSwitch {
			checked: !SettingsModel.useAnimations
			//: DESKTOP
			text: qsTr("Use images instead of animations")

			onCheckedChanged: SettingsModel.useAnimations = !checked
		}
		GSwitch {
			checked: SettingsModel.visualPrivacy
			//: DESKTOP
			text: qsTr("Hide key animations when entering PIN")

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
		GSwitch {
			checked: !SettingsModel.autoRedirectAfterAuthentication
			//: DESKTOP
			description: qsTr("After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.")
			drawBottomCorners: true
			//: DESKTOP
			text: qsTr("Manual redirection back to the provider")

			onCheckedChanged: SettingsModel.autoRedirectAfterAuthentication = !checked
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		contentSpacing: 0
		//: DESKTOP
		title: qsTr("Behavior")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GSwitch {
			checked: SettingsModel.autoStartApp
			//: DESKTOP Description for auto-start option
			description: qsTr("The %1 gets started on system boot, so that it can be opened automatically on an authentication. It has to be started manually otherwise.").arg(Qt.application.name)
			enabled: !SettingsModel.autoStartSetByAdmin && SettingsModel.autoStartAvailable
			//: DESKTOP Text for auto-start option
			text: qsTr("Automatically start %1 (recommended)").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoStartApp = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			checked: SettingsModel.trayIconEnabled
			//: MACOS Description for attaching the AA to the menu bar/system tray
			description: qsTr("The %1 continues to run in the background after the application window is closed, so that it can be opened automatically on an authentication.").arg(Qt.application.name)
			text: Qt.platform.os === "osx" ?
			//: MACOS Text for attaching the AA to the menu bar
			qsTr("Attach %1 to menu bar (recommended)").arg(Qt.application.name) :
			//: WINDOWS Text for attaching the AA to the system tray
			qsTr("Attach %1 to system tray (recommended)").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.trayIconEnabled = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			checked: SettingsModel.autoCloseWindowAfterAuthentication
			drawBottomCorners: !updateOptions.visible

			//: DESKTOP
			text: qsTr("Close %1 window after authentication").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		UpdateOptions {
			id: updateOptions

			visible: SettingsModel.autoUpdateAvailable

			onShowUpdateRequested: root.showUpdateRequested()
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		//: DESKTOP
		title: qsTr("Network")
		visible: SettingsModel.customProxyAttributesPresent

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GSwitch {
			checked: SettingsModel.useCustomProxy
			drawBottomCorners: true

			//: DESKTOP
			text: qsTr("Use the proxy (%1) specified during the installation.").arg(SettingsModel.customProxyUrl)

			onCheckedChanged: SettingsModel.useCustomProxy = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
}
