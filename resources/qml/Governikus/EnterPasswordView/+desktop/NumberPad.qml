/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style
import Governikus.Type.SettingsModel

Item {
	id: baseItem

	property alias deleteEnabled: deleteButton.enabled
	property alias submitAccessibleText: submitButton.a11yText
	property alias submitEnabled: submitButton.enabled

	signal deletePressed
	signal digitPressed(string digit)
	signal submitPressed

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
	RoundedRectangle {
		id: numPadContainer

		anchors.bottom: parent.bottom
		anchors.left: parent.left
		borderColor: Style.color.pane_border
		borderWidth: Math.max(1, plugin.scaleFactor * 3)
		bottomLeftCorner: false
		bottomRightCorner: false
		color: Style.color.pane
		height: grid.height + 2 * Constants.pane_padding
		topLeftCorner: false
		visible: SettingsModel.useScreenKeyboard
		width: grid.width + 2 * Constants.pane_padding

		layer {
			enabled: GraphicsInfo.api !== GraphicsInfo.Software

			effect: GDropShadow {
			}
		}
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

					onClicked: baseItem.digitPressed(text)
				}
			}
			NumberPadButton {
				id: deleteButton

				//: LABEL DESKTOP A11y text for the "delete" button text when the button is disabled.
				a11yDisabledText: qsTr("Delete last digit, disabled until input is present.")
				//: LABEL DESKTOP A11y text for the "delete" button image.
				a11yText: qsTr("Delete last digit")
				enabled: baseItem.deleteEnabled
				icon.source: "qrc:///images/material_backspace.svg"

				onClicked: baseItem.deletePressed()
			}
			NumberPadButton {
				id: zeroButton

				text: d.numbers[9]

				onClicked: baseItem.digitPressed(text)
			}
			NumberPadButton {
				id: submitButton

				//: LABEL DESKTOP A11y text, appended onto the "submit" button text when the button is disabled.
				a11yDisabledText: a11yText + qsTr(", disabled until input is complete.")
				//: LABEL DESKTOP A11y text for the "submit" button image.
				a11yText: qsTr("Submit")
				enabled: baseItem.submitEnabled
				icon.source: "qrc:///images/material_check.svg"

				onClicked: baseItem.submitPressed()
			}
		}
	}
}
