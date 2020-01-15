/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
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

	onVisibleChanged: if (visible && ApplicationModel.bluetoothAvailable) deprecationWarning.open()

	ConfirmationPopup {
		id: deprecationWarning

		style: ConfirmationPopup.PopupStyle.OkButton
		title: qsTr("Deprecation warning") + SettingsModel.translationTrigger
		text: qsTr("Bluetooth support will be removed in the next version of %1!").arg(Qt.application.name) + SettingsModel.translationTrigger
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
		anchors.leftMargin: 5
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: technologySwitch.top

		enableButtonVisible: ApplicationModel.bluetoothAvailable && (!ApplicationModel.bluetoothEnabled || parent.showLocationPermissionInfo)
		enableButtonText: (!ApplicationModel.bluetoothEnabled ?
						   //: LABEL ANDROID IOS
						   qsTr("Enable Bluetooth") :
						   //: LABEL ANDROID IOS
						   qsTr("Continue")
						  ) + SettingsModel.translationTrigger
		onEnableClicked: {
			if (!ApplicationModel.bluetoothEnabled) {
				ApplicationModel.bluetoothEnabled = true
			} else {
				parent.locationPermissionInfoConfirmed = true
			}
		}
		enableText: (!visible ? ""
				  : !ApplicationModel.bluetoothAvailable ?
					//: INFO ANDROID IOS Bluetooth is not available to AA2, the user is requested to use NFC instead.
					qsTr("Bluetooth is not supported by your device.") + "<br/>" +
					//: INFO ANDROID IOS Bluetooth is not available to AA2, the user is requested to use NFC instead.
					qsTr("Please try NFC.")
				  : !ApplicationModel.bluetoothEnabled ?
					//: INFO ANDROID IOS Bluetooth is available but not active.
					qsTr("Bluetooth is switched off.") + "<br/>" +
					//: INFO ANDROID IOS Bluetooth is available but not active.
					qsTr("Please enable Bluetooth.")
					//: INFO ANDROID IOS Bluetooth is active but the AA2 does not have the location permission which is required to find the bluetooth reader. The user is asked to give the permission to the AA2 after pressing the "ok" button.
				  : parent.showLocationPermissionInfo ? qsTr("No paired and activated Bluetooth device has been detected. To discover available devices the location permission has to be granted to the AusweisApp2. You can grant this permission after clicking the continue button.")
					//: INFO ANDROID IOS The connection to the bluetooth reader failed.
				  : !ApplicationModel.bluetoothResponding ? qsTr("An error occured while connecting to your bluetooth card reader. Please pair your card reader in the system settings and restart the app.")
				  : "") + SettingsModel.translationTrigger

		//: INFO ANDROID IOS Status message while connecting to the bluetooth reader - Header
		titleText: ((baseItem.waitingFor === Workflow.WaitingFor.Reader) ? qsTr("Establishing connection...")
				//: INFO ANDROID IOS Status message while the connection to the bluetooth has been established and an ID card needs to be inserted - Header
				 : (baseItem.waitingFor === Workflow.WaitingFor.Card) ? qsTr("Determining card...")
				 : "") + SettingsModel.translationTrigger

		//: LABEL ANDROID IOS
		subTitleText: (!visible ? ""
					: !!NumberModel.inputError ? NumberModel.inputError
					//: INFO ANDROID IOS Status message while the connection to the bluetooth reader is being established.
					: (baseItem.waitingFor === Workflow.WaitingFor.Reader) ? qsTr("Searching for card reader...")
					//: INFO ANDROID IOS Status message after the connection to the bluetooth reader is established and an ID card needs to be inserted.
					: (baseItem.waitingFor === Workflow.WaitingFor.Card) ? qsTr("Please insert your ID card into your card reader.")
					: "") + SettingsModel.translationTrigger
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
