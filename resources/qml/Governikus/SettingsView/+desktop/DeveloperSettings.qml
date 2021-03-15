/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts 1.12

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

		//: LABEL DESKTOP_QML
		text: qsTr("Developer options")
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Testmode for the self-authentication")
		checked: SettingsModel.useSelfauthenticationTestUri
		onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Developer mode")
		checked: SettingsModel.developerMode
		onCheckedChanged: SettingsModel.developerMode = checked
	}

	GText {
		Layout.fillWidth: true

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("The developer mode is aimed at integrators / developers for new service applications. For this reason, the developer mode works only in the test PKI. By activating the developer mode, some safety tests are deactivated. This means that the authentication process continues although the AusweisApp2 would usually abort the process with an error message when used in normal operation mode. Information on the disregarded error in the developer mode is displayed in the attached window below the AusweisApp2.")
		textStyle: Style.text.hint_warning

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}
}
