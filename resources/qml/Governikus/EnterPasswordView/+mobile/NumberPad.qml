/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

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
	Layout.fillHeight: false
	Layout.fillWidth: false
	columnSpacing: 10
	columns: 3
	height: width
	rowSpacing: columnSpacing
	width: 250

	QtObject {
		id: d

		readonly property var numbers: {
			var numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0];
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
			text: d.numbers[index]
			visualPrivacy: SettingsModel.visualPrivacy

			onClicked: baseItem.digitPressed(text)
		}
	}
	NumberPadButton {
		a11yDisabledText: qsTr("Delete last digit, disabled until input is present.")
		enabled: baseItem.deleteEnabled
		icon.source: "qrc:///images/mobile/material_backspace.svg"
		text: qsTr("Delete last digit")

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
		//: LABEL ANDROID IOS A11y text, appended onto the "submit" button text when the button is disabled.
		a11yDisabledText: a11yText + qsTr(", disabled until input is complete.")
		a11yText: submitAccessibleText !== "" ? submitAccessibleText : text
		enabled: baseItem.submitEnabled
		icon.source: "qrc:///images/material_check.svg"
		text: qsTr("Submit")

		onClicked: baseItem.submitPressed()
	}
}
