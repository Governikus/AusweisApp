import QtQuick 2.5
import QtQuick.Controls 1.4

Item {
	id: baseItem
	signal abortNfc

	state: "initial"
	states: [
		State { name: "nfc_connect"
			PropertyChanges { target: switchToBluetoothAction; visible: true }
			PropertyChanges { target: nfcConnectionIndication; visible: true }
		},
		State { name: "nfc_card"
			PropertyChanges { target: switchToBluetoothAction; visible: true }
			PropertyChanges { target: nfcConnectionIndication; visible: true }
		},
		State { name: "nfc_updateretrycounter"
			PropertyChanges { target: switchToBluetoothAction; visible: false }
			PropertyChanges { target: nfcConnectionIndication; visible: false }
		}
	]

	NfcConnectionIndication {
		id: nfcConnectionIndication
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.bottom: switchToBluetoothAction.top
	}

	SwitchToBluetooth {
		id: switchToBluetoothAction
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		onClicked: parent.abortNfc()
	}
}
