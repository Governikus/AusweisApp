/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
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
		contentSpacing: 0
		//: DESKTOP
		title: qsTr("Numeric keypad")

		GSwitch {
			checked: SettingsModel.shuffleScreenKeyboard
			//: DESKTOP
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")

			//: DESKTOP
			text: qsTr("Shuffle keys")

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}
		GSwitch {
			checked: SettingsModel.visualPrivacy
			//: DESKTOP
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")
			drawBottomCorners: true

			//: DESKTOP
			text: qsTr("Hide key animations")

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
	}
}
