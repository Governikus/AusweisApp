/*
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ReaderPlugIn 1.0


Item {
	id: baseItem

	height: technologyRow.height

	signal requestPluginType(int pReaderPlugInType)

	property int selectedTechnology

	Row {
		id: technologyRow

		spacing: Constants.component_spacing * 2
		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter

		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.NFC
			onClicked: baseItem.requestPluginType(ReaderPlugIn.NFC)
			imageSource: "qrc:///images/mobile/icon_nfc.svg"
			//: LABEL ANDROID IOS
			text: qsTr("NFC")
		}

		TechnologySwitchButton {
			buttonActive: selectedTechnology !== ReaderPlugIn.REMOTE
			onClicked: baseItem.requestPluginType(ReaderPlugIn.REMOTE)
			imageSource: "qrc:///images/mobile/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("WiFi")
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
