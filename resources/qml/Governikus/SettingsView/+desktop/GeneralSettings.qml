/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Language selection") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	RowLayout {
		spacing: Constants.component_spacing

		LocationButton {
			Accessible.name: qsTr("German") + SettingsModel.translationTrigger
			Accessible.description: qsTr("Set language to german") + SettingsModel.translationTrigger

			language: "de"
			text: "Deutsch"
			image: "qrc:///images/location_flag_de.svg"
		}

		LocationButton {
			Accessible.name: qsTr("English") + SettingsModel.translationTrigger
			Accessible.description: qsTr("Set language to english") + SettingsModel.translationTrigger

			language: "en"
			text: "English"
			image: "qrc:///images/location_flag_en.svg"
		}

	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("UI settings") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	RowLayout {
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GButton {
			//: LABEL DESKTOP_QML
			text: qsTr("Switch back to old UI") + SettingsModel.translationTrigger
			onClicked: plugin.switchUi()
		}

		GText {
			Layout.fillWidth: true

			activeFocusOnTab: true

			text: qsTr("Please note that the old UI will be removed in the next version of %1!").arg(Qt.application.name) + SettingsModel.translationTrigger
			textStyle: Style.text.normal_warning

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Behavior") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Auto start AusweisApp2 after boot") + SettingsModel.translationTrigger
		checked: SettingsModel.autoStartApp
		enabled: !SettingsModel.autoStartSetByAdmin && SettingsModel.autoStartAvailable
		onCheckedChanged: SettingsModel.autoStartApp = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Close after authentication") + SettingsModel.translationTrigger
		checked: SettingsModel.autoCloseWindowAfterAuthentication
		onCheckedChanged: SettingsModel.autoCloseWindowAfterAuthentication = checked
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Use internal notifications") + SettingsModel.translationTrigger
		checked: SettingsModel.showInAppNotifications
		enabled: !SettingsModel.developerMode
		onCheckedChanged: SettingsModel.showInAppNotifications = checked
	}

	GText {
		visible: SettingsModel.developerMode
		Layout.fillWidth: true

		activeFocusOnTab: true

		//: LABEL DESKTOP_QML Only visible when the user activates the developer mode in the settings.
		text: qsTr("Using the developer mode forces the notifications to be enabled.") + SettingsModel.translationTrigger
		textStyle: Style.text.hint_warning

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}
}
