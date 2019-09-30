/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("History") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Save authentification history") + SettingsModel.translationTrigger
		checked: SettingsModel.historyEnabled
		onCheckedChanged: SettingsModel.historyEnabled = checked
	}

	GButton {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Clear entire history") + SettingsModel.translationTrigger
		onClicked: SettingsModel.removeEntireHistory()
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("Onscreen keypad") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Use on screen keypad for PIN entry") + SettingsModel.translationTrigger
		checked: SettingsModel.useScreenKeyboard
		onCheckedChanged: SettingsModel.useScreenKeyboard = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Shuffle keypad buttons") + SettingsModel.translationTrigger
		checked: SettingsModel.shuffleScreenKeyboard
		enabled: SettingsModel.useScreenKeyboard
		onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("Software updates") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Check at program start") + SettingsModel.translationTrigger
		checked: SettingsModel.autoUpdateCheck
		enabled: !SettingsModel.autoUpdateCeckSetByAdmin
		onCheckedChanged: SettingsModel.autoUpdateCheck = checked
	}

	GButton {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Check now") + SettingsModel.translationTrigger
		onClicked: SettingsModel.updateApp()
	}

}
