/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
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

		//: LABEL DESKTOP
		text: qsTr("History")
		textStyle: Style.text.header_accent

		FocusFrame {
		}
	}
	GCheckBox {
		checked: SettingsModel.historyEnabled

		//: LABEL DESKTOP
		text: qsTr("Save authentication history")

		onCheckedChanged: SettingsModel.historyEnabled = checked
	}
	GButton {
		disabledTooltipText: qsTr("History is empty")
		enableButton: !HistoryModel.empty
		//: LABEL DESKTOP
		text: qsTr("Clear entire history")

		onClicked: confirmationPopup.open()
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Onscreen keypad")
		textStyle: Style.text.header_accent

		FocusFrame {
		}
	}
	GCheckBox {
		checked: SettingsModel.useScreenKeyboard

		//: LABEL DESKTOP
		text: qsTr("Use on screen keypad for PIN entry")

		onCheckedChanged: SettingsModel.useScreenKeyboard = checked
	}
	GCheckBox {
		checked: SettingsModel.shuffleScreenKeyboard
		enabled: SettingsModel.useScreenKeyboard

		//: LABEL DESKTOP
		text: qsTr("Shuffle keypad buttons")

		onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
	}
	GCheckBox {
		checked: !SettingsModel.visualPrivacy
		enabled: SettingsModel.useScreenKeyboard

		//: LABEL DESKTOP
		text: qsTr("Visual feedback when pressing keypad buttons")

		onCheckedChanged: SettingsModel.visualPrivacy = !checked
	}
	GSeparator {
		Layout.fillWidth: true
	}
	GText {
		activeFocusOnTab: true

		//: LABEL DESKTOP
		text: qsTr("Software updates")
		textStyle: Style.text.header_accent

		FocusFrame {
		}
	}
	GCheckBox {
		checked: SettingsModel.autoUpdateCheck
		enabled: !SettingsModel.autoUpdateCheckSetByAdmin && SettingsModel.autoUpdateAvailable

		//: LABEL DESKTOP
		text: qsTr("Check at program start")

		onCheckedChanged: SettingsModel.autoUpdateCheck = checked
	}
	RowLayout {
		readonly property bool updateAvailable: SettingsModel.appUpdateData.updateAvailable
		readonly property bool updateValid: SettingsModel.appUpdateData.valid

		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GButton {
			enabled: SettingsModel.autoUpdateAvailable
			text: (parent.updateAvailable ?
				//: LABEL DESKTOP
				qsTr("Show update") :
				//: LABEL DESKTOP
				qsTr("Check now"))

			onClicked: SettingsModel.updateAppcast()
		}
		GText {
			Layout.fillWidth: true
			activeFocusOnTab: true
			text: {
				if (parent.updateAvailable && parent.updateValid) {
					//: LABEL DESKTOP An update is available, the new version is supplied to the user.
					return qsTr("An update is available (version %1)!").arg(SettingsModel.appUpdateData.version);
				} else if (parent.updateAvailable && !parent.updateValid) {
					//: LABEL DESKTOP The updater found an update but not all required update information are valid, this should be a very rare case.
					return qsTr("An update is available but retrieving the information failed.");
				} else if (!parent.updateAvailable && parent.updateValid) {
					//: LABEL DESKTOP The current version is up to date, no user action is required.
					return qsTr("Your version %1 of %2 is up to date.").arg(Qt.application.version).arg(Qt.application.name);
				} else {
					//: LABEL DESKTOP The automatic update check is disabled (or no network connection was present during app start), a manual check for update is required.
					return qsTr("No update information available, please check for update manually.");
				}
			}
			textStyle: (parent.updateAvailable || !parent.updateValid) ? Style.text.normal_warning : Style.text.normal_accent

			FocusFrame {
			}
		}
	}
	ConfirmationPopup {
		id: confirmationPopup
		//: INFO DESKTOP The current history is about to be removed, user confirmation required.
		text: qsTr("All history entries will be deleted.")

		//: LABEL DESKTOP
		title: qsTr("Delete history")

		onConfirmed: {
			let removedItemCount = SettingsModel.removeEntireHistory();
			//: INFO DESKTOP Feedback how many history entries were removed.
			ApplicationModel.showFeedback(qsTr("Deleted %1 entries from the history.").arg(removedItemCount));
		}
	}
}
