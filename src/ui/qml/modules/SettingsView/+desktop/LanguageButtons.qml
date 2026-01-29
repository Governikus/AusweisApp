/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick.Layouts

import Governikus.Global
import Governikus.Type

RowLayout {
	id: root

	spacing: 0

	GRepeater {
		id: repeater

		delegate: GRadioButton {
			required property int index
			required property string value

			checked: SettingsModel.language === value
			position: getPosition(index, repeater.count)
			tintIcon: false

			onToggled: SettingsModel.language = value
		}
		model: LanguageButtonData {
		}
	}
}
