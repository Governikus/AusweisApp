/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View
import Governikus.Type.SettingsModel

ColumnLayout {
	spacing: Constants.component_spacing

	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Change language")

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

		DarkModeButtons {
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useSystemFont

			//: LABEL DESKTOP
			description: qsTr("Toggling will restart the %1").arg(Qt.application.name)
			//: LABEL DESKTOP
			text: qsTr("Use the system font")

			onCheckedChanged: {
				if (checked !== SettingsModel.useSystemFont) {
					SettingsModel.useSystemFont = checked;
					plugin.doRefresh();
				}
			}
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useAnimations
			drawBottomCorners: true
			//: LABEL DESKTOP
			text: qsTr("Play animations")

			onCheckedChanged: SettingsModel.useAnimations = checked
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: 0
		//: LABEL DESKTOP
		title: qsTr("Behavior")

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.autoStartApp
			enabled: !SettingsModel.autoStartSetByAdmin && SettingsModel.autoStartAvailable
			text: Qt.platform.os === "osx" ?
			//: LABEL MACOS Text for auto-start option
			qsTr("Auto-start %1 after boot and add to menu bar").arg(Qt.application.name) :
			//: LABEL WINDOWS Text for auto-start option
			qsTr("Auto-start %1 after boot and add a tray icon").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoStartApp = checked
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.autoCloseWindowAfterAuthentication

			//: LABEL DESKTOP
			text: qsTr("Close %1 after authentication").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
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
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Network")
		visible: SettingsModel.customProxyAttributesPresent

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useCustomProxy
			drawBottomCorners: true

			//: LABEL DESKTOP
			text: qsTr("Use the proxy (%1) specified during the installation.").arg(SettingsModel.customProxyUrl)

			onCheckedChanged: SettingsModel.useCustomProxy = checked
		}
	}
}
