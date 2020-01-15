/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

GridLayout {
	id: baseItem

	property bool deleteEnabled: true
	property bool submitEnabled: true

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

		text: qsTr("Delete last digit") + SettingsModel.translationTrigger
		icon.source: "qrc:///images/delete.svg"

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

		text: qsTr("Submit") + SettingsModel.translationTrigger
		icon.source: "qrc:///images/submit.svg"

		onClicked: baseItem.submitPressed()
	}
}
