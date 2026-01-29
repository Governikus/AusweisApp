/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Type

GRepeater {
	id: root

	signal buttonClicked

	delegate: GRadioButton {
		required property int value

		checked: SettingsModel.userDarkMode === value
		tintIcon: true

		onClicked: {
			if (SettingsModel.userDarkMode === value)
				return;

			SettingsModel.userDarkMode = value;
			root.buttonClicked();
		}
	}
	model: DarkModeButtonData {
	}
}
