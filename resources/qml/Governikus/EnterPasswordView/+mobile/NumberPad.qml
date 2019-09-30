/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.Type.SettingsModel 1.0

GridLayout {
	id: baseItem

	property bool deleteEnabled: true
	property bool submitEnabled: true

	signal digitPressed(string digit)
	signal deletePressed()
	signal submitPressed()

	columns: 3
	columnSpacing: 10
	rowSpacing: columnSpacing
	width: 250
	height: width
	Layout.fillHeight: false
	Layout.fillWidth: false

	NumberPadButton { text: "1"; onClicked: baseItem.digitPressed("1"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "2"; onClicked: baseItem.digitPressed("2"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "3"; onClicked: baseItem.digitPressed("3"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "4"; onClicked: baseItem.digitPressed("4"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "5"; onClicked: baseItem.digitPressed("5"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "6"; onClicked: baseItem.digitPressed("6"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "7"; onClicked: baseItem.digitPressed("7"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "8"; onClicked: baseItem.digitPressed("8"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton { text: "9"; onClicked: baseItem.digitPressed("9"); Layout.fillHeight: true; Layout.fillWidth: true }
	NumberPadButton {
		visible: baseItem.deleteEnabled
		Layout.fillHeight: true
		Layout.fillWidth: true

		Accessible.name: qsTr("Delete last digit") + SettingsModel.translationTrigger

		source: "qrc:///images/delete.svg"

		onClicked: baseItem.deletePressed()
	}
	NumberPadButton {
		Layout.fillHeight: true
		Layout.fillWidth: true
		Layout.column: 1
		Layout.row: 3

		text: "0"

		onClicked: baseItem.digitPressed("0")
	}
	NumberPadButton {
		visible: baseItem.submitEnabled
		Layout.fillHeight: true
		Layout.fillWidth: true

		Accessible.name: qsTr("Submit") + SettingsModel.translationTrigger

		source: "qrc:///images/submit.svg"

		onClicked: baseItem.submitPressed()
	}
}
