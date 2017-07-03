import QtQuick 2.5
import QtQuick.Controls 1.4

import "../"

SectionPage
{
	id: baseItem
	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: {changePinModel.cancelWorkflow()} }
	headerTitleBarAction: TitleBarAction { text: qsTr("Pin"); font.bold: true }

	NfcWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("nfc_") === 0
		onAbortNfc: {
			changePinModel.abortCardSelection()
			changePinModel.setReaderType("BLUETOOTH");
		}
	}


	BluetoothWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("bt_") === 0
	}
}
