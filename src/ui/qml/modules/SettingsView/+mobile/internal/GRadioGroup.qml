/**
 * Copyright (c) 2025-2026 Governikus Service GmbH, Germany
 */

import QtQuick.Controls

import Governikus.Global
import Governikus.Type

GCollapsible {
	id: root

	function onOptionSelected() {
		if (expanded && !ApplicationModel.screenReaderRunning) {
			expanded = false;
		}
	}

	//: MOBILE
	a11yDescription: description !== "" ? qsTr("Currently selected is %1").arg(description) : ""
	description: radioGroup.checkedButton ? radioGroup.checkedButton.text : ""
	icon: radioGroup.checkedButton ? radioGroup.checkedButton.icon.source : ""

	ButtonGroup {
		id: radioGroup

		buttons: root.content.filter(child => child instanceof GRadioButton)

		onButtonsChanged: buttons.forEach(button => button.ButtonGroup.group = this)
	}
}
