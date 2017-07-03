import QtQuick 2.5
import QtQuick.Controls 1.4

import "../"

SectionPage
{
	id: baseItem
	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: {changePinModel.cancelWorkflow()} }
	headerTitleBarAction: TitleBarAction { text: qsTr("PIN Management"); font.bold: true }

	NfcWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("nfc_") === 0
		onAbortNfc: {
			changePinControllern.readerType = "BLUETOOTH"
			changePinModel.abortCardSelection()
		}
	}


	BluetoothWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("bt_") === 0
		onAbortBluetooth: {
			changePinControllern.readerType = "NFC"
			changePinModel.abortCardSelection()
		}
	}
}
