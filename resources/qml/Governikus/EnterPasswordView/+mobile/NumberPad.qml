/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

GridLayout {
	id: baseItem

	property bool deleteEnabled: true
	property bool submitEnabled: true

	Accessible.role: Accessible.Grouping
	//: LABEL ANDROID IOS
	Accessible.description: qsTr("Number pad")
	Accessible.focusable: true

	signal digitPressed(string digit)
	signal deletePressed()
	signal submitPressed()

	QtObject {
		id: d

		readonly property var numbers: {
			var numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0]
			if (visible && SettingsModel.shuffleScreenKeyboard) {
				Utils.shuffle(numbers)
			}
			return numbers
		}
	}

	columns: 3
	columnSpacing: 10
	rowSpacing: columnSpacing
	width: 250
	height: width
	Layout.fillHeight: false
	Layout.fillWidth: false

	Repeater {
		id: numberRepeater

		model: 9

		NumberPadButton {
			text: d.numbers[index];
			visualPrivacy: SettingsModel.visualPrivacy

			onClicked: baseItem.digitPressed(text)
		}
	}

	NumberPadButton {
		enabled: baseItem.deleteEnabled

		text: qsTr("Delete last digit")
		a11yDisabledText: qsTr("Delete last digit, disabled until input is present.")
		icon.source: "qrc:///images/mobile/material_backspace.svg"

		onClicked: baseItem.deletePressed()
	}

	NumberPadButton {
		Layout.column: 1
		Layout.row: 3

		text: d.numbers[9]
		visualPrivacy: SettingsModel.visualPrivacy

		onClicked: baseItem.digitPressed(text)
	}

	NumberPadButton {
		enabled: baseItem.submitEnabled

		text: qsTr("Submit")
		a11yDisabledText: qsTr("Submit, disabled until input is complete.")
		icon.source: "qrc:///images/material_check.svg"

		onClicked: baseItem.submitPressed()
	}
}
