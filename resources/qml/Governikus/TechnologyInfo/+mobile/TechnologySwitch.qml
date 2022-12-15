/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ReaderPlugIn 1.0

Item {
	id: baseItem

	property int selectedTechnology
	property var supportedTechnologies

	signal requestPluginType(int pReaderPlugInType)

	height: technologyRow.height

	Row {
		id: technologyRow
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		spacing: Constants.component_spacing * 2

		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.NFC
			imageSource: "qrc:///images/mobile/icon_nfc.svg"
			//: LABEL ANDROID IOS
			text: qsTr("NFC")
			visible: supportedTechnologies.includes(ReaderPlugIn.NFC)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.NFC)
		}
		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.SMART
			imageSource: "qrc:///images/mobile/icon_smart.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SMART")
			visible: supportedTechnologies.includes(ReaderPlugIn.SMART)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.SMART)
		}
		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.REMOTE_IFD
			imageSource: "qrc:///images/mobile/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("WiFi")
			visible: supportedTechnologies.includes(ReaderPlugIn.REMOTE_IFD)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.REMOTE_IFD)
		}
		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.SIMULATOR
			imageSource: "qrc:///images/mobile/icon_simulator.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SIM")
			visible: supportedTechnologies.includes(ReaderPlugIn.SIMULATOR)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.SIMULATOR)
		}
	}
	GSeparator {
		width: technologyRow.width * 1.5

		anchors {
			horizontalCenter: technologyRow.horizontalCenter
			top: technologyRow.top
		}
	}
}
