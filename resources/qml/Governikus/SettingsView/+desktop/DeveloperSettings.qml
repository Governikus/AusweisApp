/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {
	readonly property string helpTopic: "settingsDeveloper"

	spacing: Constants.component_spacing

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Developer options")
		textStyle: Style.text.header_accent

		FocusFrame {}
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Testmode for the self-authentication")
		checked: SettingsModel.useSelfauthenticationTestUri
		onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
	}

	ToggleableOption {
		//: LABEL DESKTOP
		text: qsTr("Enable internal card simulator")
		checked: SettingsModel.enableSimulator
		onCheckedChanged: SettingsModel.enableSimulator = checked
	}

	GText {
		Layout.fillWidth: true

		activeFocusOnTab: true
		//: LABEL DESKTOP
		text: qsTr("The internal card simulator allows to run an authentication in the test PKI without any ID card or card reader. Note that no other card reader can be used while the simulator is activated.")
		textStyle: Style.text.hint_warning

		FocusFrame {}
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Developer mode")
		checked: SettingsModel.developerMode
		onCheckedChanged: SettingsModel.developerMode = checked
	}

	GText {
		Layout.fillWidth: true

		activeFocusOnTab: true
		//: LABEL DESKTOP
		text: qsTr("The developer mode is aimed at integrators / developers for new service applications. For this reason, the developer mode works only in the test PKI. By activating the developer mode, some safety tests are deactivated. This means that the authentication process continues although the AusweisApp2 would usually abort the process with an error message when used in normal operation mode. Information on the disregarded error in the developer mode is displayed in the attached window below the AusweisApp2.")
		textStyle: Style.text.hint_warning

		FocusFrame {}
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Custom config.json")
		textStyle: Style.text.header_accent

		FocusFrame {}
	}

	GText {
		Layout.fillWidth: true

		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Place the config.json into the application folder to override the embedded config.")
		textStyle: Style.text.normal

		FocusFrame {}
	}

	GText {
		Layout.fillWidth: true

		//: LABEL DESKTOP
		text: qsTr("Application folder: %1").arg(ApplicationModel.customConfigPath)
		textStyle: Style.text.hint_secondary
	}

	GButton {
		//: LABEL DESKTOP
		text: qsTr("Save config.json")
		onClicked: {
			let filenameSuggestion = "config"
			fileDialog.selectFile(filenameSuggestion)
		}

		GFileDialog {
			id: fileDialog

			folder: ApplicationModel.customConfigPath
			defaultSuffix: "json"
			//: LABEL DESKTOP
			nameFilters: qsTr("JSON config (*.json)")

			onAccepted: ApplicationModel.saveEmbeddedConfig(file)
		}
	}
}
