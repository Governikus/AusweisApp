import QtQuick 2.5
import QtQuick.Controls 1.4

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0

SectionPage
{
	id: baseItem

	leftTitleBarAction: TitleBarAction {
		enabled: !(baseItem.state === "changepin_enterpin" ||
				   baseItem.state === "changepin_entercan")
		state: enabled ? "cancel" : "hidden"
		onClicked: changePinModel.cancelWorkflow()
	}
	headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management") + settingsModel.translationTrigger; font.bold: true }

	NfcWorkflow
	{
		allowRemote: false
		anchors.fill: parent
		state: parent.state
		visible: changePinModel.readerPlugInType === "NFC"
		onRequestPluginType: changePinModel.readerPlugInType = pReaderPlugInType;
	}

	RemoteWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: changePinModel.readerPlugInType === "REMOTE" || changePinModel.readerPlugInType === "PCSC"
		onRequestPluginType: changePinModel.readerPlugInType = pReaderPlugInType;
	}

	BluetoothWorkflow
	{
		allowRemote: false
		anchors.fill: parent
		state: parent.state
		visible: changePinModel.readerPlugInType === "BLUETOOTH"
		onRequestPluginType: changePinModel.readerPlugInType = pReaderPlugInType;
	}
}
