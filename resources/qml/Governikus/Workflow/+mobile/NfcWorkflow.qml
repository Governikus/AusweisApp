/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem
	signal requestPluginType(int pReaderPlugInType)
	clip: true

	Item {
		id: progressIndicatorWrapper
		height: 2 * baseItem.height / 5
		anchors.left: parent.left
		anchors.right: parent.right

		NfcProgressIndicator {
			id: progressIndicator
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: baseItem.height / 2
			state: visible && ApplicationModel.nfcAvailable && ApplicationModel.nfcEnabled ? "on" : "off"
		}
	}


	TechnologyInfo {
		id: technologyInfo

		anchors.left: parent.left
		anchors.leftMargin: 5
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicatorWrapper.bottom
		anchors.bottom: technologySwitch.top
		clip: true

		enableButtonVisible: ApplicationModel.nfcAvailable && (!ApplicationModel.nfcEnabled || !ApplicationModel.nfcRunning)
		//: LABEL DESKTOP_QML
		enableButtonText: (!ApplicationModel.nfcEnabled ? qsTr("Go to NFC settings") : qsTr("Start NFC scan")) + SettingsModel.translationTrigger
		onEnableClicked: !ApplicationModel.nfcEnabled ? ApplicationModel.showSettings(ApplicationModel.SETTING_NFC) : ApplicationModel.nfcRunning = true
		enableText: (!visible ? "" :
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use bluetooth instead.
					!ApplicationModel.nfcAvailable ? qsTr("NFC is not supported by your device.") + "<br/>" + qsTr("You require an additional 'Bluetooth card reader' or an additional 'smartphone as card reader' to use the online identification function with this device.") :
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					!ApplicationModel.nfcEnabled ? qsTr("NFC is switched off.") + "<br/>" + qsTr("Please enable NFC in your system settings.") :
					 //: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					 !ApplicationModel.nfcRunning ? qsTr("NFC scan is not running.") + "<br/>" + qsTr("Please start the NFC scan.") : ""
					) + SettingsModel.translationTrigger

		titleText: qsTr("Establish connection") + SettingsModel.translationTrigger

		subTitleText: (!visible ? "" :
					//: INFO ANDROID IOS The NFC interface does not meet the minimum requirements, using a bluetooth reader or a differnt smarthpone is suggested.
					  ApplicationModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). You require an additional 'Bluetooth card reader' or an additional 'smartphone as card reader' to use the online identification function with this device.") :
					//: INFO ANDROID IOS The online authentication feature is disabled and needs to be activated by the authorities.
					  NumberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.") :
					//: INFO ANDROID IOS The id card may be inserted, the authentication process may be started.
					  qsTr("Please place your device<br/>on your ID card.")
					  ) + SettingsModel.translationTrigger
	}


	TechnologySwitch {
		id: technologySwitch
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: ReaderPlugIn.NFC
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}
}
