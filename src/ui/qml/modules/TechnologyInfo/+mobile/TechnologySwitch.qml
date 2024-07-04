/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

GFlickableColumnLayout {
	id: baseItem

	property bool flowVertically: false
	readonly property int maxItemHeight: Math.max(nfc.implicitHeight, smart.implicitHeight, remote.implicitHeight, sim.implicitHeight)
	readonly property int maxItemWidth: Math.max(nfc.implicitWidth, smart.implicitWidth, remote.implicitWidth, sim.implicitWidth)
	property int selectedTechnology
	property var supportedTechnologies

	signal requestPluginType(int pReaderPluginType)

	clip: true
	implicitHeight: maxItemHeight + 2 * Constants.pane_padding
	implicitWidth: maxItemWidth + 2 * Constants.pane_padding
	leftMargin: 0
	rightMargin: 0

	QtObject {
		id: d

		//: INFO ANDROID IOS A11y text on button indicating the currently active technology. %1 will be replaced with the name of the technology (e.g. NFC).
		readonly property string activatedTechnologyA11yText: qsTr("%1 mode is active.")
		//: INFO ANDROID IOS A11y text on button to switch to another technology. %1 will be replaced with the name of the technology (e.g. NFC).
		readonly property string deactivatedTechnologyA11yText: qsTr("Click here to change the mode to %1.")

		function getTechnologyA11yText(pChecked, pTechnologyName) {
			return pChecked ? d.activatedTechnologyA11yText.arg(pTechnologyName) : d.deactivatedTechnologyA11yText.arg(pTechnologyName);
		}
	}
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

			Accessible.name: d.getTechnologyA11yText(checked, text)
			checkable: !checked
			checked: selectedTechnology === ReaderManagerPluginType.NFC
			icon.source: "qrc:///images/mobile/icon_nfc.svg"
			//: LABEL ANDROID IOS
			text: qsTr("NFC")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderManagerPluginType.NFC)

			onClicked: baseItem.requestPluginType(ReaderManagerPluginType.NFC)
		}
		GButton {
			id: smart

			Accessible.name: d.getTechnologyA11yText(checked, text)
			checkable: !checked
			checked: selectedTechnology === ReaderManagerPluginType.SMART
			icon.source: "qrc:///images/mobile/icon_smart.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SMART")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderManagerPluginType.SMART)

			onClicked: baseItem.requestPluginType(ReaderManagerPluginType.SMART)
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GButton {
			id: remote

			Accessible.name: d.getTechnologyA11yText(checked, text)
			checkable: !checked
			checked: selectedTechnology === ReaderManagerPluginType.REMOTE_IFD
			icon.source: "qrc:///images/mobile/icon_remote.svg"
			//: LABEL ANDROID IOS
			text: qsTr("WiFi")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderManagerPluginType.REMOTE_IFD)

			onClicked: baseItem.requestPluginType(ReaderManagerPluginType.REMOTE_IFD)
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GButton {
			id: sim

			Accessible.name: d.getTechnologyA11yText(checked, text)
			checkable: !checked
			checked: selectedTechnology === ReaderManagerPluginType.SIMULATOR
			icon.source: "qrc:///images/mobile/icon_simulator.svg"
			//: LABEL ANDROID IOS
			text: qsTr("SIM")
			tintIcon: true
			visible: supportedTechnologies.includes(ReaderManagerPluginType.SIMULATOR)

			onClicked: baseItem.requestPluginType(ReaderManagerPluginType.SIMULATOR)
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
	}
	GSpacer {
		Layout.fillHeight: true
		visible: flowVertically
	}
}
