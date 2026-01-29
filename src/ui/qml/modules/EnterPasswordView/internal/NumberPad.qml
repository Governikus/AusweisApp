/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Type
import Governikus.View

Item {
	id: root

	property bool deleteEnabled: true
	property alias submitAccessibleText: submitButton.a11yText
	property bool submitEnabled: true

	signal deletePressed
	signal digitPressed(string digit)
	signal submitPressed

	Layout.fillHeight: true
	Layout.fillWidth: true
	Layout.maximumHeight: buttonLayout.Layout.maximumWidth
	Layout.maximumWidth: buttonLayout.Layout.maximumWidth
	Layout.minimumHeight: buttonLayout.Layout.minimumHeight
	Layout.minimumWidth: buttonLayout.Layout.minimumWidth
	implicitHeight: buttonLayout.implicitHeight
	implicitWidth: buttonLayout.implicitWidth

	GridLayout {
		id: buttonLayout

		Accessible.focusable: true
		//: MOBILE
		Accessible.name: qsTr("Number pad")
		Accessible.role: Accessible.Grouping
		activeFocusOnTab: true
		anchors.fill: parent
		columnSpacing: 10
		columns: 3
		rowSpacing: columnSpacing

		Keys.onPressed: event => {
			if (!buttonLayout.activeFocus) {
				event.accepted = false;
			} else if (event.key >= Qt.Key_0 && event.key <= Qt.Key_9) {
				root.digitPressed((event.key - Qt.Key_0).toString());
				event.accepted = true;
			} else if (event.key === Qt.Key_Backspace || event.key === Qt.Key_Delete) {
				root.deletePressed();
				event.accepted = true;
			} else if ((event.key === Qt.Key_Enter || event.key === Qt.Key_Return) && root.submitEnabled) {
				root.submitPressed();
				event.accepted = true;
			} else {
				event.accepted = false;
			}
		}

		QtObject {
			id: d

			readonly property var numbers: {
				let numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 0];
				if (root.visible && SettingsModel.shuffleScreenKeyboard) {
					Utils.shuffle(numbers);
				}
				return numbers;
			}
		}
		Repeater {
			id: numberRepeater

			model: 9

			NumberPadButton {
				required property int index

				Layout.fillHeight: true
				Layout.fillWidth: true
				text: d.numbers[index]

				onClicked: root.digitPressed(text)
			}
		}
		NumberPadButton {
			Layout.fillHeight: true
			Layout.fillWidth: true

			//: MOBILE A11y text for the "delete" button text when the button is disabled.
			a11yDisabledText: qsTr("Delete last digit, disabled until input is present.")
			//: MOBILE A11y text for the "delete" button image.
			a11yText: qsTr("Delete last digit")
			enabled: root.deleteEnabled
			icon.source: "qrc:///images/material_backspace.svg"

			onClicked: root.deletePressed()
		}
		NumberPadButton {
			Layout.column: 1
			Layout.fillHeight: true
			Layout.fillWidth: true
			Layout.row: 3
			text: d.numbers[9]

			onClicked: root.digitPressed(text)
		}
		SubmitButton {
			id: submitButton

			Layout.fillHeight: true
			Layout.fillWidth: true

			//: MOBILE A11y text, appended onto the "submit" button text when the button is disabled.
			a11yDisabledText: a11yText + qsTr(", disabled until input is complete.")
			//: MOBILE A11y text for the "submit" button image.
			a11yText: qsTr("Submit")
			enabled: root.submitEnabled
			icon.source: "qrc:///images/material_check.svg"

			onClicked: root.submitPressed()
		}
	}
	FocusFrame {
		scope: buttonLayout
	}
}
