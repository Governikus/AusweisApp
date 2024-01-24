/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ReaderPlugIn

GFlickableColumnLayout {
	id: baseItem

	property bool flowVertically: false
	readonly property int maxItemHeight: Math.max(nfc.implicitHeight, smart.implicitHeight, remote.implicitHeight, sim.implicitHeight)
	readonly property int maxItemWidth: Math.max(nfc.implicitWidth, smart.implicitWidth, remote.implicitWidth, sim.implicitWidth)
	property int selectedTechnology
	property var supportedTechnologies

	signal requestPluginType(int pReaderPlugInType)

	clip: true
	implicitHeight: maxItemHeight + 2 * Constants.pane_padding
	implicitWidth: maxItemWidth + 2 * Constants.pane_padding
	leftMargin: 0
	rightMargin: 0

	GSpacer {
		Layout.fillHeight: true
		visible: flowVertically
	}
	GridLayout {
		id: switcher

		readonly property int columnCount: 1 + (baseItem.width - maxItemWidth) / (maxItemWidth + columnSpacing)

		Layout.alignment: Qt.AlignCenter
		columnSpacing: Constants.component_spacing * 2
		columns: flowVertically ? 1 : columnCount
		rowSpacing: Constants.component_spacing

		GButton {
			id: nfc

			checkable: !checked
			checked: selectedTechnology === ReaderPlugIn.NFC
			icon.source: "qrc:///images/mobile/icon_nfc.svg"
			//: LABEL ANDROID IOS
			text: qsTr("NFC")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderPlugIn.NFC)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.NFC)
		}
		GButton {
			id: smart

			checkable: !checked
			checked: selectedTechnology === ReaderPlugIn.SMART
			icon.source: "qrc:///images/mobile/icon_smart.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SMART")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderPlugIn.SMART)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.SMART)
		}
		GButton {
			id: remote

			checkable: !checked
			checked: selectedTechnology === ReaderPlugIn.REMOTE_IFD
			icon.source: "qrc:///images/mobile/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("WiFi")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderPlugIn.REMOTE_IFD)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.REMOTE_IFD)
		}
		GButton {
			id: sim

			checkable: !checked
			checked: selectedTechnology === ReaderPlugIn.SIMULATOR
			icon.source: "qrc:///images/mobile/icon_simulator.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SIM")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderPlugIn.SIMULATOR)

			onClicked: baseItem.requestPluginType(ReaderPlugIn.SIMULATOR)
		}
	}
	GSpacer {
		Layout.fillHeight: true
		visible: flowVertically
	}
}
