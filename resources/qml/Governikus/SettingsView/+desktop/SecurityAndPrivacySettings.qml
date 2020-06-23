/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.View 1.0

ColumnLayout {
	readonly property string helpTopic: "settingsSecurityPrivacy"

	spacing: Constants.component_spacing

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("History") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Save authentication history") + SettingsModel.translationTrigger
		checked: SettingsModel.historyEnabled
		onCheckedChanged: SettingsModel.historyEnabled = checked
	}

	GButton {
		//: LABEL DESKTOP_QML
		text: qsTr("Clear entire history") + SettingsModel.translationTrigger
		onClicked: confirmationPopup.open()
		enableButton: !HistoryModel.empty
		disabledTooltipText: qsTr("History is empty") + SettingsModel.translationTrigger
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Onscreen keypad") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
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

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Visual feedback when pressing keypad buttons") + SettingsModel.translationTrigger
		checked: !SettingsModel.visualPrivacy
		enabled: SettingsModel.useScreenKeyboard
		onCheckedChanged: SettingsModel.visualPrivacy = !checked
	}

	GSeparator {
		Layout.fillWidth: true
	}

	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Software updates") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	ToggleableOption {
		activeFocusOnTab: true

		//: LABEL DESKTOP_QML
		text: qsTr("Check at program start") + SettingsModel.translationTrigger
		checked: SettingsModel.autoUpdateCheck
		enabled: !SettingsModel.autoUpdateCheckSetByAdmin && SettingsModel.autoUpdateAvailable
		onCheckedChanged: SettingsModel.autoUpdateCheck = checked
	}

	RowLayout {
		readonly property bool updateAvailable: SettingsModel.appUpdateData.updateAvailable
		readonly property bool updateValid: SettingsModel.appUpdateData.valid

		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GButton {
			text: (parent.updateAvailable ?
				   //: LABEL DESKTOP_QML
				   qsTr("Show update") :
				   //: LABEL DESKTOP_QML
				   qsTr("Check now")
				   ) + SettingsModel.translationTrigger
			enabled: SettingsModel.autoUpdateAvailable
			onClicked: SettingsModel.updateApp()
		}

		GText {
			Layout.fillWidth: true

			activeFocusOnTab: true

			text: {
				SettingsModel.translationTrigger

				if (parent.updateAvailable && parent.updateValid) {
					//: LABEL DESKTOP_QML An update is available, the new version is supplied to the user.
					return qsTr("An update is available (version %1)!").arg(SettingsModel.appUpdateData.version)
				}
				else if (parent.updateAvailable && !parent.updateValid) {
					//: LABEL DESKTOP_QML The updater found an update but not all required update information are valid, this should be a very rare case.
					return qsTr("An update is available but retrieving the information failed.")
				}
				else if (!parent.updateAvailable && parent.updateValid) {
					//: LABEL DESKTOP_QML The current version is up to date, no user action is required.
					return qsTr("Your version %1 of %2 is up to date.").arg(Qt.application.version).arg(Qt.application.name)
				}
				else {
					//: LABEL DESKTOP_QML The automatic update check is disabled (or no network connection was present during app start), a manual check for update is required.
					return qsTr("No update information available, please check for update manually.")
				}
			}
			textStyle: (parent.updateAvailable || !parent.updateValid) ? Style.text.normal_warning : Style.text.normal_accent

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}
	}

	ConfirmationPopup {
		id: confirmationPopup

		//: LABEL DESKTOP_QML
		title: qsTr("Delete history")+ SettingsModel.translationTrigger
		//: INFO DESKTOP_QML The current history is about to be removed, user confirmation required.
		text: qsTr("All history entries will be deleted.") + SettingsModel.translationTrigger
		onConfirmed: {
			let removedItemCount = SettingsModel.removeEntireHistory()
			//: INFO DESKTOP_QML Feedback how many history entries were removed.
			ApplicationModel.showFeedback(qsTr("Deleted %1 entries from the history.").arg(removedItemCount))
		}
	}
}
