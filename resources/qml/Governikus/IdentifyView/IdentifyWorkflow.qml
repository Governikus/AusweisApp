import QtQuick 2.5
import QtQuick.Controls 1.4

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Workflow 1.0

SectionPage
{
	id: baseItem

	leftTitleBarAction: TitleBarAction {
		enabled: !(baseItem.state === "identify_enterpin" ||
				   baseItem.state === "identify_entercan")
		state: enabled ? "cancel" : "hidden"
		onClicked: authModel.cancelWorkflow()
	}
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify") + settingsModel.translationTrigger; font.bold: true }

	NfcWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: authModel.readerPlugInType === "NFC"
		onRequestPluginType: authModel.readerPlugInType = pReaderPlugInType;
	}

	RemoteWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: authModel.readerPlugInType === "REMOTE" || authModel.readerPlugInType === "PCSC"
		onRequestPluginType: authModel.readerPlugInType = pReaderPlugInType;
	}

	BluetoothWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: authModel.readerPlugInType === "BLUETOOTH"
		onRequestPluginType: authModel.readerPlugInType = pReaderPlugInType;
	}
}
