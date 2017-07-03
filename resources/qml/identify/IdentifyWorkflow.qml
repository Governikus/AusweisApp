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
			authModel.abortCardSelection()
			authModel.setReaderType("BLUETOOTH");
		}
	}


	BluetoothWorkflow
	{
		anchors.fill: parent
		state: parent.state
		visible: parent.state.indexOf("bt_") === 0
	}
}
