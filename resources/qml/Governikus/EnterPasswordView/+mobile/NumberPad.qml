/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Type.SettingsModel

GridLayout {
	id: baseItem

	property bool deleteEnabled: true
	property string submitAccessibleText
	property bool submitEnabled: true

	signal deletePressed
	signal digitPressed(string digit)
	signal submitPressed

	//: LABEL ANDROID IOS
	Accessible.description: qsTr("Number pad")
	Accessible.focusable: true
	Accessible.role: Accessible.Grouping
	columnSpacing: 10
	columns: 3
	rowSpacing: columnSpacing

	QtObject {
		id: d

		readonly property var numbers: {
			let numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0];
			if (visible && SettingsModel.shuffleScreenKeyboard) {
				Utils.shuffle(numbers);
			}
			return numbers;
		}
	}
	Repeater {
		id: numberRepeater

		model: 9

		NumberPadButton {
			Layout.fillHeight: true
			Layout.fillWidth: true
			text: d.numbers[index]

			onClicked: baseItem.digitPressed(text)
		}
	}
	NumberPadButton {
		Layout.fillHeight: true
		Layout.fillWidth: true

		//: LABEL ANDROID IOS A11y text for the "delete" button text when the button is disabled.
		a11yDisabledText: qsTr("Delete last digit, disabled until input is present.")
		enabled: baseItem.deleteEnabled
		icon.source: "qrc:///images/mobile/material_backspace.svg"
		text: qsTr("Delete last digit")

		onClicked: baseItem.deletePressed()
	}
	NumberPadButton {
		Layout.column: 1
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.row: 3
		text: d.numbers[9]

		onClicked: baseItem.digitPressed(text)
	}
	NumberPadButton {
		Layout.fillHeight: true
		Layout.fillWidth: true

		//: LABEL ANDROID IOS A11y text, appended onto the "submit" button text when the button is disabled.
		a11yDisabledText: a11yText + qsTr(", disabled until input is complete.")
		a11yText: submitAccessibleText !== "" ? submitAccessibleText : text
		enabled: baseItem.submitEnabled
		icon.source: "qrc:///images/material_check.svg"
		text: qsTr("Submit")

		onClicked: baseItem.submitPressed()
	}
}
