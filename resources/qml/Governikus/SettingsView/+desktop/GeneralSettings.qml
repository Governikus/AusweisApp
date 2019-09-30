/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("Language selection") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	RowLayout {
		spacing: Constants.component_spacing

		LocationButton {
			activeFocusOnTab: true
			Accessible.name: qsTr("German") + SettingsModel.translationTrigger
			Accessible.description: qsTr("Set language to german") + SettingsModel.translationTrigger

			language: "de"
			text: "DE"
			image: "qrc:///images/location_flag_de.svg"
		}

		LocationButton {
			activeFocusOnTab: true
			Accessible.name: qsTr("English") + SettingsModel.translationTrigger
			Accessible.description: qsTr("Set language to english") + SettingsModel.translationTrigger

			language: "en"
			text: "EN"
			image: "qrc:///images/location_flag_en.svg"
		}

	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("UI settings") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	GButton {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Switch back to old UI") + SettingsModel.translationTrigger
		onClicked: plugin.switchUi()
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("Behavior") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Auto start AusweisApp2 after boot") + SettingsModel.translationTrigger
		checked: SettingsModel.autoStartApp
		enabled: !SettingsModel.autoStartSetByAdmin
		onCheckedChanged: SettingsModel.autoStartApp = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Close after authentication") + SettingsModel.translationTrigger
		checked: SettingsModel.autoCloseWindowAfterAuthentication
		onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
	}

}
