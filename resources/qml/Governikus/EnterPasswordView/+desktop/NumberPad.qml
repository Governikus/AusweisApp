/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Item {
	id: baseItem

	property alias deleteEnabled: deleteButton.enabled
	property alias submitEnabled: submitButton.enabled

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

	Rectangle {
		id: numPadContainer

		visible: SettingsModel.useScreenKeyboard
		width: grid.width + 2 * Constants.pane_padding
		height: grid.height + 2 * Constants.pane_padding

		anchors.right: parent.right
		anchors.bottom: parent.bottom

		color: Qt.lighter(Style.color.background, 1.15)
		border.width: Math.max(1, ApplicationModel.scaleFactor * 3)
		border.color: Style.color.accent

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

				text: "C"
				color: "yellow"
				fontScale: 0.75
				onClicked: baseItem.deletePressed()
				enabled: baseItem.deleteEnabled
			}

			NumberPadButton {
				id: zeroButton

				text: d.numbers[9]
				visualPrivacy: SettingsModel.visualPrivacy
				onClicked: baseItem.digitPressed(text)
			}

			NumberPadButton {
				id: submitButton

				text: "OK"
				color: "lime"
				fontScale: 0.75
				onClicked: baseItem.submitPressed()
				enabled: baseItem.submitEnabled
			}
		}
	}
}
