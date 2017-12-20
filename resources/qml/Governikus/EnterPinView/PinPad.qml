import QtQuick 2.5
import QtQuick.Layouts 1.1

import Governikus.Global 1.0

GridLayout {
	id: baseItem

	property bool deleteEnabled: true
	property bool submitEnabled: true

	signal digitPressed(string digit)
	signal deletePressed()
	signal submitPressed()

	columns: 3
	columnSpacing: Utils.dp(10)
	rowSpacing: columnSpacing
	width: Utils.dp(250)
	height: width
	Layout.fillHeight: false
	Layout.fillWidth: false

	PinPadButton { text: "1"; onClicked: baseItem.digitPressed("1"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "2"; onClicked: baseItem.digitPressed("2"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "3"; onClicked: baseItem.digitPressed("3"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "4"; onClicked: baseItem.digitPressed("4"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "5"; onClicked: baseItem.digitPressed("5"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "6"; onClicked: baseItem.digitPressed("6"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "7"; onClicked: baseItem.digitPressed("7"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "8"; onClicked: baseItem.digitPressed("8"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton { text: "9"; onClicked: baseItem.digitPressed("9"); Layout.fillHeight: true; Layout.fillWidth: true }
	PinPadButton {
		source: baseItem.deleteEnabled ? "qrc:///images/delete.svg" : ""
		onClicked: baseItem.deletePressed()
		enabled: baseItem.deleteEnabled
		Layout.fillHeight: true; Layout.fillWidth: true
	}
	PinPadButton { text: "0"; onClicked: baseItem.digitPressed("0"); Layout.fillHeight: true; Layout.fillWidth: true  }
	PinPadButton {
		source: baseItem.submitEnabled ? "qrc:///images/submit.svg" : ""
		onClicked: baseItem.submitPressed()
		enabled: baseItem.submitEnabled
		Layout.fillHeight: true; Layout.fillWidth: true
	}
}
