/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ReaderPlugIn 1.0


Item {
	id: baseItem
	height: technologyRow.height

	signal requestPluginType(int pReaderPlugInType)

	property int selectedTechnology

	Row {
		id: technologyRow
		spacing: 20
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.NFC
			onClicked: baseItem.requestPluginType(ReaderPlugIn.NFC)
			imageSource: "qrc:///images/icon_nfc.svg"
			//: LABEL ANDROID IOS
			text: qsTr("NFC") + SettingsModel.translationTrigger
		}

		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.REMOTE
			onClicked: baseItem.requestPluginType(ReaderPlugIn.REMOTE)
			imageSource: "qrc:///images/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("WiFi") + SettingsModel.translationTrigger
		}

		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.BLUETOOTH
			onClicked: baseItem.requestPluginType(ReaderPlugIn.BLUETOOTH)
			imageSource: "qrc:///images/icon_bluetooth.svg"
			//: LABEL ANDROID IOS
			text: qsTr("Bluetooth") + SettingsModel.translationTrigger
		}
	}
}
