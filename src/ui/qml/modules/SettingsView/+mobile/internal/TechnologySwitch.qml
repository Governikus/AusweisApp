/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Type

GCollapsible {
	id: root

	function getImage(pTechnology) {
		switch (pTechnology) {
		case ReaderManagerPluginType.NFC:
			return "qrc:///images/mobile/icon_nfc.svg";
		case ReaderManagerPluginType.SMART:
			return "qrc:///images/mobile/icon_smart.svg";
		case ReaderManagerPluginType.REMOTE_IFD:
			return "qrc:///images/mobile/icon_remote.svg";
		case ReaderManagerPluginType.SIMULATOR:
			return "qrc:///images/mobile/icon_simulator.svg";
		}
	}
	function getLabel(pTechnology) {
		switch (pTechnology) {
		case ReaderManagerPluginType.NFC:
			//: LABEL ANDROID IOS
			return qsTr("by NFC");
		case ReaderManagerPluginType.SMART:
			//: LABEL ANDROID IOS
			return qsTr("by Smart-eID");
		case ReaderManagerPluginType.REMOTE_IFD:
			//: LABEL ANDROID IOS
			return qsTr("by smartphone as card reader");
		case ReaderManagerPluginType.SIMULATOR:
			//: LABEL ANDROID IOS
			return qsTr("by internal card simulator");
		}
	}

	selectionIcon: getImage(SettingsModel.preferredTechnology)
	selectionTitle: getLabel(SettingsModel.preferredTechnology)
	tintIcon: true
	//: LABEL ANDROID IOS
	title: qsTr("Readout mode")
	width: parent.width

	TechnologyButton {
		technology: ReaderManagerPluginType.NFC
	}
	TechnologyButton {
		technology: ReaderManagerPluginType.SMART
		visible: ApplicationModel.isSmartSupported
	}
	TechnologyButton {
		drawBottomCorners: !SettingsModel.enableSimulator
		technology: ReaderManagerPluginType.REMOTE_IFD
	}
	TechnologyButton {
		drawBottomCorners: true
		technology: ReaderManagerPluginType.SIMULATOR
		visible: SettingsModel.enableSimulator
	}

	component TechnologyButton: GCollapsibleSubButton {
		required property int technology

		Layout.fillWidth: true
		image: root.getImage(technology)
		text: root.getLabel(technology)
		tintIcon: true

		onClicked: {
			SettingsModel.preferredTechnology = technology;
			root.expanded = false;
		}
	}
}
