/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0

Item {
	id: baseItem

	property alias deleteEnabled: deleteButton.enabled
	property string submitAccessibleText
	property alias submitEnabled: submitButton.enabled

	signal deletePressed
	signal digitPressed(string digit)
	signal submitPressed

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
	Rectangle {
		id: numPadContainer
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		border.color: Style.color.accent
		border.width: Math.max(1, ApplicationModel.scaleFactor * 3)
		color: Qt.darker(Style.color.background, 1.05)
		height: grid.height + 2 * Constants.pane_padding
		visible: SettingsModel.useScreenKeyboard
		width: grid.width + 2 * Constants.pane_padding

		Grid {
			id: grid
			anchors.centerIn: parent
			columns: 3
			spacing: Constants.component_spacing

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
				id: deleteButton
				color: Constants.red
				enabled: baseItem.deleteEnabled
				fontScale: 0.75
				text: "C"

				onClicked: baseItem.deletePressed()
			}
			NumberPadButton {
				id: zeroButton
				text: d.numbers[9]
				visualPrivacy: SettingsModel.visualPrivacy

				onClicked: baseItem.digitPressed(text)
			}
			NumberPadButton {
				id: submitButton
				Accessible.name: submitAccessibleText !== "" ? submitAccessibleText : text
				color: Constants.green
				enabled: baseItem.submitEnabled
				fontScale: 0.75
				text: "OK"

				onClicked: baseItem.submitPressed()
			}
		}
	}
}
