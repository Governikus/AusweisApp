import QtQuick 2.5
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0

Item {
	id: baseItem
	signal requestPluginType(string pReaderPlugInType)

	property alias allowRemote: technologySwitch.allowRemote

	property bool locationPermissionInfoConfirmed: false
	onLocationPermissionInfoConfirmedChanged: {
		if (identifyController) identifyController.locationPermissionConfirmed = locationPermissionInfoConfirmed
		if (changePinController) changePinController.locationPermissionConfirmed = locationPermissionInfoConfirmed
	}

	readonly property bool showLocationPermissionInfo: applicationModel.locationPermissionRequired && !locationPermissionInfoConfirmed

	onStateChanged: {
		if (state === "reader") {
			locationPermissionInfoConfirmed = false
		}
	}

	onShowLocationPermissionInfoChanged: {
		if (applicationModel.bluetoothEnabled && !showLocationPermissionInfo) {
			numberModel.continueWorkflow()
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
		state: (!applicationModel.bluetoothAvailable || !applicationModel.bluetoothEnabled || parent.showLocationPermissionInfo) ? "off" :
			   (baseItem.state === "reader") ? "one" :
			   (baseItem.state === "card") ? "two" : ""
	}

	TechnologyInfo {
		id: techInfo
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: technologySwitch.top
		state: parent.state

		enableButtonVisible: applicationModel.bluetoothAvailable && (!applicationModel.bluetoothEnabled || parent.showLocationPermissionInfo)
		enableButtonText: (!applicationModel.bluetoothEnabled ? qsTr("Enable Bluetooth") : qsTr("Continue")) + settingsModel.translationTrigger
		onEnableClicked: {
			if (!applicationModel.bluetoothEnabled) {
				applicationModel.bluetoothEnabled = true
			} else {
				parent.locationPermissionInfoConfirmed = true
			}
		}
		enableText: (!visible ? "" :
					!applicationModel.bluetoothAvailable ? qsTr("Bluetooth is not supported by your device.") + "<br/>" + qsTr("Please try NFC.") :
					!applicationModel.bluetoothEnabled ? qsTr("Bluetooth is switched off.") + "<br/>" + qsTr("Please enable Bluetooth.") :
					parent.showLocationPermissionInfo ? qsTr("No paired and activated Bluetooth device was detected. The AusweisApp2 needs access to your location in order to discover available devices. You can grant this permission after clicking the continue button.") : ""
					) + settingsModel.translationTrigger

		titleText: ((baseItem.state === "reader") ? qsTr("Establish connection") :
				   (baseItem.state === "card") ? qsTr("Determine card") : ""
				   ) + settingsModel.translationTrigger

		subTitleText: (!visible ? "" :
					  !!numberModel.inputError ? numberModel.inputError :
					  (state === "reader") ?
							qsTr("Search card reader...") :
					  (state === "card") ?
							qsTr("Please insert your ID card.") : ""
					  ) + settingsModel.translationTrigger
		subTitleTextRedColor: false
	}

	TechnologySwitch {
		id: technologySwitch
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: "BLUETOOTH"
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}
}
