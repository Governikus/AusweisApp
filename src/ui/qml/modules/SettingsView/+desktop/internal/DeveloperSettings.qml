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

	spacing: Style.dimens.pane_spacing

	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		contentSpacing: 0
		//: DESKTOP
		title: qsTr("Developer options")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useSelfauthenticationTestUri
			//: DESKTOP
			description: qsTr("Allow test sample card usage")

			//: DESKTOP
			text: qsTr("Testmode for the self-authentication")

			onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.enableSimulator

			//: DESKTOP
			description: qsTr("The internal card simulator allows to run an authentication in the test PKI without any ID card or card reader. Note that no other card reader can be used while the simulator is activated.")
			//: DESKTOP
			text: qsTr("Internal card simulator")

			onCheckedChanged: SettingsModel.enableSimulator = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.developerMode

			//: DESKTOP
			description: qsTr("The developer mode deactivates some security checks and the authentication process will continue even if some errors occur. Skipped errors will be shown as notifications. The developer mode is only usable with the test PKI.")
			drawBottomCorners: true
			//: DESKTOP
			text: qsTr("Developer mode")

			onCheckedChanged: SettingsModel.developerMode = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.showInAppNotifications

			//: DESKTOP Only visible when the user activates the developer mode in the settings.
			description: SettingsModel.developerMode ? qsTr("Using the developer mode forces the notifications to be enabled.") : ""
			drawBottomCorners: true
			enabled: !SettingsModel.developerMode

			//: DESKTOP
			text: qsTr("Show notifications inside of %1").arg(Qt.application.name)

			onCheckedChanged: SettingsModel.showInAppNotifications = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
	GPane {
		Layout.fillWidth: true
		//: DESKTOP
		title: qsTr("Custom config.json")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GText {
			//: DESKTOP
			text: qsTr("Place the config.json into the application folder to override the embedded config.")
		}
		GText {
			//: DESKTOP
			text: qsTr("Application folder: %1").arg(ApplicationModel.customConfigPath)
		}
		GButton {
			//: DESKTOP
			text: qsTr("Save config.json")

			onClicked: {
				let filenameSuggestion = "config";
				fileDialog.selectFile(filenameSuggestion);
			}

			GFileDialog {
				id: fileDialog

				currentFolder: ApplicationModel.customConfigPath
				defaultSuffix: "json"
				//: DESKTOP
				nameFilters: qsTr("JSON config (*.json)")

				//: DESKTOP
				title: qsTr("Save config.json")

				onAccepted: ApplicationModel.saveEmbeddedConfig(selectedFile)
			}
		}
	}
}
