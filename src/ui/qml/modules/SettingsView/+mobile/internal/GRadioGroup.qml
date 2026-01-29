/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Controls

import Governikus.Global

GCollapsible {
	id: root

	selectionIcon: radioGroup.checkedButton ? radioGroup.checkedButton.icon.source : ""
	selectionTitle: radioGroup.checkedButton ? radioGroup.checkedButton.text : ""

	ButtonGroup {
		id: radioGroup

		buttons: root.content.filter(child => child instanceof GRadioButton)
	}
}
