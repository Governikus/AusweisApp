/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick

import Governikus.Global
import Governikus.Type

GRepeater {
	id: root

	signal buttonClicked

	delegate: GRadioButton {
		required property string value

		checked: SettingsModel.language === value

		onClicked: {
			if (SettingsModel.language === value)
				return;

			SettingsModel.language = value;
			root.buttonClicked();
		}
	}
	model: LanguageButtonData {
	}
}
