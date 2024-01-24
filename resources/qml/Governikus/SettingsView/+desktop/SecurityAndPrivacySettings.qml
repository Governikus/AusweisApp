/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.SettingsModel
import Governikus.View

ColumnLayout {
	spacing: Constants.component_spacing

	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Onscreen keypad")

		LabeledSwitch {
			checked: SettingsModel.useScreenKeyboard

			//: LABEL DESKTOP
			title: qsTr("Use on screen keypad for PIN entry")

			onCheckedChanged: SettingsModel.useScreenKeyboard = checked
		}
		LabeledSwitch {
			checked: SettingsModel.shuffleScreenKeyboard
			enabled: SettingsModel.useScreenKeyboard

			//: LABEL DESKTOP
			title: qsTr("Shuffle digits of on screen keypad")

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}
		LabeledSwitch {
			checked: !SettingsModel.visualPrivacy
			//: LABEL DESKTOP
			description: qsTr("Visually highlight key presses on screen keypad")
			enabled: SettingsModel.useScreenKeyboard

			//: LABEL DESKTOP
			title: qsTr("Button animation")

			onCheckedChanged: SettingsModel.visualPrivacy = !checked
		}
	}
	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing
		//: LABEL DESKTOP
		title: qsTr("Software updates")
		visible: SettingsModel.autoUpdateAvailable

		LabeledSwitch {
			checked: SettingsModel.autoUpdateCheck
			enabled: !SettingsModel.autoUpdateCheckSetByAdmin

			//: LABEL DESKTOP
			title: qsTr("Check at program start")

			onCheckedChanged: SettingsModel.autoUpdateCheck = checked
		}
		RowLayout {
			readonly property bool updateAvailable: SettingsModel.appUpdateData.updateAvailable
			readonly property bool updateValid: SettingsModel.appUpdateData.valid

			Layout.fillWidth: true
			spacing: Constants.component_spacing

			GButton {
				text: (parent.updateAvailable ?
					//: LABEL DESKTOP
					qsTr("Show update") :
					//: LABEL DESKTOP
					qsTr("Check now"))

				onClicked: SettingsModel.updateAppcast()
			}
			GText {
				activeFocusOnTab: true
				color: (parent.updateAvailable || !parent.updateValid) ? Style.color.text_warning : Style.color.control
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
				textStyle: Style.text.normal

				FocusFrame {
				}
			}
		}
	}
}
