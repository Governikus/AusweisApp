/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

ColumnLayout {
	spacing: Style.dimens.pane_spacing

	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: 0
		//: LABEL DESKTOP
		title: qsTr("Numeric keypad")

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.shuffleScreenKeyboard
			//: LABEL DESKTOP
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")

			//: LABEL DESKTOP
			text: qsTr("Shuffle keys")

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}
		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.visualPrivacy
			//: LABEL DESKTOP
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")
			drawBottomCorners: true

			//: LABEL DESKTOP
			text: qsTr("Hide key animations")

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
	}
	GPane {
		Layout.fillWidth: true
		contentPadding: 0
		spacing: Style.dimens.pane_spacing
		//: LABEL DESKTOP
		title: qsTr("Software updates")
		visible: SettingsModel.autoUpdateAvailable

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.autoUpdateCheck
			enabled: !SettingsModel.autoUpdateCheckSetByAdmin

			//: LABEL DESKTOP
			text: qsTr("Check for updates at program start")

			onCheckedChanged: SettingsModel.autoUpdateCheck = checked
		}
		RowLayout {
			readonly property bool updateAvailable: SettingsModel.appUpdateData.updateAvailable
			readonly property bool updateValid: SettingsModel.appUpdateData.valid

			Layout.bottomMargin: Style.dimens.pane_padding
			Layout.leftMargin: Style.dimens.pane_padding
			Layout.rightMargin: Style.dimens.pane_padding
			spacing: Style.dimens.pane_spacing

			GButton {
				text: (parent.updateAvailable ?
					//: LABEL DESKTOP
					qsTr("Show update") :
					//: LABEL DESKTOP
					qsTr("Check now"))

				onClicked: SettingsModel.updateAppcast()
			}
			GText {
				color: (parent.updateAvailable || !parent.updateValid) ? Style.color.textNormal.basic : Style.color.textSubline.basic
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
			}
		}
	}
}
