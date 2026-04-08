/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick.Layouts

import Governikus.Global
import Governikus.Type

RowLayout {
	function onChanged(pChecked, pMode) {
		if (!pChecked || SettingsModel.userDarkMode === pMode)
			return;
		SettingsModel.userDarkMode = pMode;
	}

	spacing: 0

	GRepeater {
		id: repeater

		delegate: GRadioButton {
			required property int index
			required property int value

			Layout.fillWidth: false
			checked: SettingsModel.userDarkMode === value
			position: getPosition(index, repeater.count)

			onToggled: SettingsModel.userDarkMode = value
		}
		model: DarkModeButtonData {
		}
	}
}
