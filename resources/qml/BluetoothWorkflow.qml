import QtQuick 2.5
import QtQuick.Layouts 1.1

import "global"

Item {
	id: baseItem
	signal abortBluetooth

	readonly property bool showLocationPermissionInfo: applicationModel.locationPermissionRequired && !locationPermissionInfoConfirmed
	property bool locationPermissionInfoConfirmed: false

	onStateChanged: {
		if (state === "bt_connect") {
			locationPermissionInfoConfirmed = false
		}
	}

	onShowLocationPermissionInfoChanged: {
		if (applicationModel.bluetoothEnabled && !showLocationPermissionInfo) {
			numberModel.continueWorkflow()
		}
	}

	function isInProcessingState() {
		return state === "bt_enternewpin" || state === "bt_updateretrycounter" ||
				state === "bt_enterpuk" || state === "bt_changepin_entercan" ||
				state === "bt_changepin_enterpin" || state === "bt_identify_entercan" ||
				state === "bt_identify_enterpin"
	}

	BluetoothProgressIndicator {
		id: progressIndicator
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		height: parent.height / 2
		state: (!applicationModel.bluetoothAvailable || !applicationModel.bluetoothEnabled || parent.showLocationPermissionInfo) ? "off" :
			   (baseItem.state === "bt_connect") ? "one" :
			   (baseItem.state === "bt_card") ? "two" :
			   (baseItem.isInProcessingState()) ? "three" :""
	}

	TechnologyInfo {
		id: techInfo
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: switchToNfcAction.top
		state: parent.state

		enableButtonVisible: applicationModel.bluetoothAvailable && (!applicationModel.bluetoothEnabled || parent.showLocationPermissionInfo)
		enableButtonText: !applicationModel.bluetoothEnabled ? qsTr("Enable Bluetooth") : qsTr("Continue")
		onEnableClicked: {
			if (!applicationModel.bluetoothEnabled) {
				applicationModel.bluetoothEnabled = true
			} else {
				parent.locationPermissionInfoConfirmed = true
			}
		}
		enableText: !visible ? "" :
					!applicationModel.bluetoothAvailable ? qsTr("Bluetooth is not supported by your device.<br/>Please try NFC.") :
					!applicationModel.bluetoothEnabled ? qsTr("Bluetooth is switched off.<br/>Please enable Bluetooth.") :
					parent.showLocationPermissionInfo ? qsTr("No paired and activated Bluetooth device was detected. The AusweisApp2 needs access to your location in order to discover available devices. You can grant this permission after clicking the continue button.") : ""

		titleText: (baseItem.state === "bt_connect") ? qsTr("Establish connection") :
				   (baseItem.state === "bt_card") ? qsTr("Determine card") :
				   (baseItem.state === "bt_enternewpin" || baseItem.state === "bt_enterpuk"
					|| baseItem.state === "bt_changepin_entercan" || baseItem.state === "bt_changepin_enterpin") ? qsTr("Change PIN") :
				   (baseItem.state === "bt_identify_entercan" || baseItem.state === "bt_identify_enterpin") ? qsTr("Authenticate") : ""

		subTitleText: !visible ? "" :
					  !!numberModel.inputError ? numberModel.inputError :
					  numberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact your competent authority to activate the online identification function.") :
					  (state === "bt_connect") ? qsTr("Connecting...") :
					  (state === "bt_card") ? qsTr("Please insert your ID card.") :
					  (state === "bt_changepin_enterpin") ? qsTr("Please enter your old PIN or your initial transport PIN first.") :
					  (state === "bt_identify_enterpin") ? qsTr("Please enter your personal PIN.") :
					  (state === "bt_changepin_entercan"|| state === "bt_identify_entercan") ? qsTr("You have entered the wrong PIN twice. For a third attempt, you have to enter your six-digit card access number first. You can find your card access number on the front of your ID card.") :
					  (state === "bt_enterpuk") ? qsTr("You have entered a wrong PIN three times. Your PIN is now blocked. You have to enter the PUK now for unblocking.") :
					  (state === "bt_enternewpin") ? qsTr("Please enter a new arbitrary 6-digit PIN.") : ""
		subTitleTextRedColor: numberModel.inputError || numberModel.pinDeactivated || state === "bt_changepin_entercan" || state === "bt_identify_entercan" || state === "bt_enterpuk"	}

	TechnologySwitchButton {
		id: switchToNfcAction
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		onClicked: {
			parent.abortBluetooth()
		}
		imageSource: "qrc:///images/icon_nfc.svg"
		text: qsTr("Use NFC instead of<br/>Bluetooth card reader")
		enabled: parent.state === "bt_connect" || parent.state === "bt_card"
	}
}
