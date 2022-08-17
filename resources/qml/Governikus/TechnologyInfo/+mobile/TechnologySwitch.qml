/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ReaderPlugIn 1.0


Item {
	id: baseItem

	height: technologyRow.height

	signal requestPluginType(int pReaderPlugInType)

	property int selectedTechnology
	property var supportedTechnologies

	Row {
		id: technologyRow

		spacing: Constants.component_spacing * 2
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		TechnologySwitchButton {
			visible: supportedTechnologies.includes(ReaderPlugIn.NFC)
			buttonActive: selectedTechnology !== ReaderPlugIn.NFC
			onClicked: baseItem.requestPluginType(ReaderPlugIn.NFC)
			imageSource: "qrc:///images/mobile/icon_nfc.svg"
			//: LABEL ANDROID IOS
			text: qsTr("NFC")
		}

		TechnologySwitchButton {
			visible: supportedTechnologies.includes(ReaderPlugIn.SMART)
			buttonActive: selectedTechnology !== ReaderPlugIn.SMART
			onClicked: baseItem.requestPluginType(ReaderPlugIn.SMART)
			imageSource: "qrc:///images/mobile/icon_smart.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SMART")
		}

		TechnologySwitchButton {
			visible: supportedTechnologies.includes(ReaderPlugIn.REMOTE_IFD)
			buttonActive: selectedTechnology !== ReaderPlugIn.REMOTE_IFD
			onClicked: baseItem.requestPluginType(ReaderPlugIn.REMOTE_IFD)
			imageSource: "qrc:///images/mobile/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("WiFi")
		}

		TechnologySwitchButton {
			visible: supportedTechnologies.includes(ReaderPlugIn.SIMULATOR)
			buttonActive: selectedTechnology !== ReaderPlugIn.SIMULATOR
			onClicked: baseItem.requestPluginType(ReaderPlugIn.SIMULATOR)
			imageSource: "qrc:///images/mobile/icon_simulator.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SIM")
		}
	}

	GSeparator {
		width: technologyRow.width * 1.5
		anchors {
			top: technologyRow.top
			horizontalCenter: technologyRow.horizontalCenter
		}
	}
}
