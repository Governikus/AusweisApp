/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("Developer options") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Test PKI") + SettingsModel.translationTrigger
		checked: SettingsModel.useSelfauthenticationTestUri
		onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Developer mode") + SettingsModel.translationTrigger
		checked: SettingsModel.developerMode
		onCheckedChanged: SettingsModel.developerMode = checked
	}

	GText {
		Layout.fillWidth: true

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML Only visible when the user activates the developer mode in the settings.
		text: qsTr("The developer mode is aimed at integrators / developers for new service applications. For this reason, the developer mode works only in the test PKI. By activating the developer mode, some safety tests are deactivated. This means that the authentication process continues although the AusweisApp2 would usually abort the process with an error message when used in normal operation mode. Information on the disregarded error in the developer mode is displayed in the attached window below the AusweisApp2.") + SettingsModel.translationTrigger
		textStyle: Style.text.hint_warning

		FocusFrame {
			dynamic: false
			border.color: Constants.black
		}
	}
}
