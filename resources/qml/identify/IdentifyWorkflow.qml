import QtQuick 2.5
import QtQuick.Controls 1.4

import "../"
import "../global"

SectionPage
{
	id: baseItem
	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: authModel.cancelWorkflow() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }

	NfcWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("nfc_") === 0
		onAbortNfc: {
			identifyController.readerType = "BLUETOOTH"
			authModel.abortCardSelection()
		}
	}


	BluetoothWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("bt_") === 0
		onAbortBluetooth: {
			identifyController.readerType = "NFC"
			authModel.abortCardSelection()
		}
	}
}
