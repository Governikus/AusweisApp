import QtQuick 2.7

import Governikus.Global 1.0
import "." as Gov

Item {
	id: baseItem
	height: technologyRow.height

	signal requestPluginType(string pReaderPlugInType)

	property bool allowRemote: true
	property string selectedTechnology

	Row {
		id: technologyRow
		spacing: Utils.dp(20)
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		Gov.TechnologySwitchButton {
			visible: allowRemote && selectedTechnology !== "REMOTE"
			onClicked: baseItem.requestPluginType("REMOTE")
			imageSource: "qrc:///images/icon_remote.svg"
			text: qsTr("Use WiFi card reader instead<br/>of Bluetooth card reader") + settingsModel.translationTrigger

		}

		Gov.TechnologySwitchButton {
			visible: selectedTechnology !== "BLUETOOTH"
			onClicked: baseItem.requestPluginType("BLUETOOTH")
			imageSource: "qrc:///images/icon_bluetooth.svg"
			text: qsTr("Use Bluetooth card reader instead<br/>of remote card reader") + settingsModel.translationTrigger
		}
	}
}
