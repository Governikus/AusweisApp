/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
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
		title: qsTr("Language selection")

		LanguageButtons {
			columns: 4
		}
	}
	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Appearance")

		DarkModeButtons {
		}
		LabeledSwitch {
			checked: SettingsModel.useSystemFont

			//: LABEL DESKTOP
			description: qsTr("Toggling will restart the %1").arg(Qt.application.name)
			//: LABEL DESKTOP
			title: qsTr("Use the system font")

			onCheckedChanged: SettingsModel.useSystemFont = checked
		}
	}
	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Behavior")

		LabeledSwitch {
			checked: SettingsModel.autoStartApp
			enabled: !SettingsModel.autoStartSetByAdmin && SettingsModel.autoStartAvailable
			title: Qt.platform.os === "osx" ?
			//: LABEL MACOS Text for auto-start option
			qsTr("Auto-start %1 after boot and add to menu bar").arg(Qt.application.name) :
			//: LABEL WINDOWS Text for auto-start option
			qsTr("Auto-start %1 after boot and add a tray icon").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoStartApp = checked
		}
		LabeledSwitch {
			checked: SettingsModel.autoCloseWindowAfterAuthentication

			//: LABEL DESKTOP
			title: qsTr("Close %1 after authentication").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
		}
		LabeledSwitch {
			checked: SettingsModel.showInAppNotifications

			//: LABEL DESKTOP Only visible when the user activates the developer mode in the settings.
			description: SettingsModel.developerMode ? qsTr("Using the developer mode forces the notifications to be enabled.") : ""
			enabled: !SettingsModel.developerMode

			//: LABEL DESKTOP
			title: qsTr("Show notifications inside of %1").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.showInAppNotifications = checked
		}
	}
	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Network")
		visible: SettingsModel.customProxyAttributesPresent

		LabeledSwitch {
			checked: SettingsModel.useCustomProxy

			//: LABEL DESKTOP
			title: qsTr("Use the proxy (%1) specified during the installation.").arg(SettingsModel.customProxyUrl)

			onCheckedChanged: SettingsModel.useCustomProxy = checked
		}
	}
}
