/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQml
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.LogModel
import Governikus.Type.SettingsModel

ColumnLayout {
	spacing: Constants.component_spacing

	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Developer options")

		LabeledSwitch {
			checked: SettingsModel.useSelfauthenticationTestUri
			//: LABEL DESKTOP
			description: qsTr("Allow test sample card usage")

			//: LABEL DESKTOP
			title: qsTr("Testmode for the self-authentication")

			onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
		}
		LabeledSwitch {
			checked: SettingsModel.enableSimulator

			//: LABEL DESKTOP
			description: qsTr("The internal card simulator allows to run an authentication in the test PKI without any ID card or card reader. Note that no other card reader can be used while the simulator is activated.")
			//: LABEL DESKTOP
			title: qsTr("Internal card simulator")

			onCheckedChanged: SettingsModel.enableSimulator = checked
		}
		LabeledSwitch {
			checked: SettingsModel.developerMode

			//: LABEL DESKTOP
			description: qsTr("The developer mode deactivates some security checks and the authentication process will continue even if some errors occur. Skipped errors will be shown as notifications. The developer mode is only usable with the test PKI.").arg(Qt.application.name)
			//: LABEL DESKTOP
			title: qsTr("Developer mode")

			onCheckedChanged: SettingsModel.developerMode = checked
		}
	}
	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Custom config.json")

		GText {
			activeFocusOnTab: true

			//: LABEL DESKTOP
			text: qsTr("Place the config.json into the application folder to override the embedded config.")

			FocusFrame {
			}
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
