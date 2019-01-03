import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem
	signal continueWorkflow()
	signal requestPluginType(int pReaderPlugInType)

	property int waitingFor: 0

	property bool locationPermissionInfoConfirmed: false
	readonly property bool showLocationPermissionInfo: ApplicationModel.locationPermissionRequired && !locationPermissionInfoConfirmed

	onStateChanged: {
		if (baseItem.waitingFor === Workflow.WaitingFor.Reader) {
			locationPermissionInfoConfirmed = false
		}
	}

	onShowLocationPermissionInfoChanged: {
		if (ApplicationModel.bluetoothEnabled && !showLocationPermissionInfo) {
			continueWorkflow()
		}
	}

	ProgressIndicator {
		id: progressIndicator
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		height: parent.height / 2
		imageIconSource: "qrc:///images/icon_bluetooth.svg"
		imagePhoneSource: "qrc:///images/phone_bluetooth.svg"
		state: (!ApplicationModel.bluetoothAvailable || !ApplicationModel.bluetoothEnabled || parent.showLocationPermissionInfo) ? "off"
			 : (baseItem.waitingFor === Workflow.WaitingFor.Reader) ? "one"
			 : (baseItem.waitingFor === Workflow.WaitingFor.Card) ? "two"
			 : ""
	}

	TechnologyInfo {
		id: techInfo
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: technologySwitch.top

		enableButtonVisible: ApplicationModel.bluetoothAvailable && (!ApplicationModel.bluetoothEnabled || parent.showLocationPermissionInfo)
		enableButtonText: (!ApplicationModel.bluetoothEnabled ? qsTr("Enable Bluetooth") : qsTr("Continue")) + settingsModel.translationTrigger
		onEnableClicked: {
			if (!ApplicationModel.bluetoothEnabled) {
				ApplicationModel.bluetoothEnabled = true
			} else {
				parent.locationPermissionInfoConfirmed = true
			}
		}
		enableText: (!visible ? ""
				  : !ApplicationModel.bluetoothAvailable ? qsTr("Bluetooth is not supported by your device.") + "<br/>" + qsTr("Please try NFC.")
				  : !ApplicationModel.bluetoothEnabled ? qsTr("Bluetooth is switched off.") + "<br/>" + qsTr("Please enable Bluetooth.")
				  : parent.showLocationPermissionInfo ? qsTr("No paired and activated Bluetooth device was detected. The AusweisApp2 needs access to your location in order to discover available devices. You can grant this permission after clicking the continue button.")
				  : !ApplicationModel.bluetoothResponding ? qsTr("An error occured while connecting to your bluetooth device. Try to pair your device in the system settings and restart the app.")
				  : "") + settingsModel.translationTrigger

		titleText: ((baseItem.waitingFor === Workflow.WaitingFor.Reader) ? qsTr("Establish connection")
				 : (baseItem.waitingFor === Workflow.WaitingFor.Card) ? qsTr("Determine card")
				 : "") + settingsModel.translationTrigger

		subTitleText: (!visible ? ""
					: !!NumberModel.inputError ? NumberModel.inputError
					: (baseItem.waitingFor === Workflow.WaitingFor.Reader) ? qsTr("Search card reader...")
					: (baseItem.waitingFor === Workflow.WaitingFor.Card) ? qsTr("Please insert your ID card.")
					: "") + settingsModel.translationTrigger
	}

	TechnologySwitch {
		id: technologySwitch
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: ReaderPlugIn.BLUETOOTH
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}
}
