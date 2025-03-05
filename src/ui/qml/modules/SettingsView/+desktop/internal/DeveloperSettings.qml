/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

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
		spacing: 0
		//: LABEL DESKTOP
		title: qsTr("Developer options")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useSelfauthenticationTestUri
			//: LABEL DESKTOP
			description: qsTr("Allow test sample card usage")

			//: LABEL DESKTOP
			text: qsTr("Testmode for the self-authentication")

			onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.enableSimulator

			//: LABEL DESKTOP
			description: qsTr("The internal card simulator allows to run an authentication in the test PKI without any ID card or card reader. Note that no other card reader can be used while the simulator is activated.")
			//: LABEL DESKTOP
			text: qsTr("Internal card simulator")

			onCheckedChanged: SettingsModel.enableSimulator = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.developerMode

			//: LABEL DESKTOP
			description: qsTr("The developer mode deactivates some security checks and the authentication process will continue even if some errors occur. Skipped errors will be shown as notifications. The developer mode is only usable with the test PKI.")
			drawBottomCorners: true
			//: LABEL DESKTOP
			text: qsTr("Developer mode")

			onCheckedChanged: SettingsModel.developerMode = checked
			onFocusChanged: if (focus)
				Utils.positionViewAtItem(this)
		}
	}
	GPane {
		Layout.fillWidth: true
		spacing: Style.dimens.pane_spacing
		//: LABEL DESKTOP
		title: qsTr("Custom config.json")

		onFocusChanged: if (focus)
			Utils.positionViewAtItem(this)

		GText {
			//: LABEL DESKTOP
			text: qsTr("Place the config.json into the application folder to override the embedded config.")
		}
		GText {
			//: LABEL DESKTOP
			text: qsTr("Application folder: %1").arg(ApplicationModel.customConfigPath)
		}
		GButton {
			//: LABEL DESKTOP
			text: qsTr("Save config.json")

			onClicked: {
				let filenameSuggestion = "config";
				fileDialog.selectFile(filenameSuggestion);
			}

			GFileDialog {
				id: fileDialog

				defaultSuffix: "json"
				folder: ApplicationModel.customConfigPath
				//: LABEL DESKTOP
				nameFilters: qsTr("JSON config (*.json)")

				//: LABEL DESKTOP
				title: qsTr("Save config.json")

				onAccepted: ApplicationModel.saveEmbeddedConfig(file)
			}
		}
	}
}
