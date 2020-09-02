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
		enableButtonText: (!ApplicationModel.nfcEnabled ?
						   //: LABEL ANDROID IOS
						   qsTr("Go to NFC settings") :
						   //: LABEL ANDROID IOS
						   qsTr("Start NFC scan")
						  ) + SettingsModel.translationTrigger
		onEnableClicked: !ApplicationModel.nfcEnabled ? ApplicationModel.showSettings(ApplicationModel.SETTING_NFC) : ApplicationModel.nfcRunning = true
		enableText: (!visible ? "" :
					!ApplicationModel.nfcAvailable ?
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use bluetooth instead.
					qsTr("Unfortunately, this functionality is not available on your device.") + "<br/>" +
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use bluetooth instead.
					qsTr("However, you can use a separate 'smartphone as card reader' to utilize the online identification function.") :
					!ApplicationModel.nfcEnabled ?
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					qsTr("NFC is switched off.") + "<br/>" +
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					qsTr("Please enable NFC in your system settings.") :
					!ApplicationModel.nfcRunning ?
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					qsTr("NFC scan is not running.") + "<br/>" +
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					qsTr("Please start the NFC scan.") : ""
					) + SettingsModel.translationTrigger

		titleText: qsTr("Establish connection") + SettingsModel.translationTrigger

		subTitleText: (!visible ? "" :
					//: INFO ANDROID IOS The NFC interface does not meet the minimum requirements, using a bluetooth reader or a differnt smarthpone is suggested.
					  ApplicationModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). However you can use a separate 'smartphone as card reader' to utilize the online identification function.") :
					//: INFO ANDROID IOS The online authentication feature is disabled and needs to be activated by the authorities.
					  NumberModel.pinDeactivated ? qsTr("The online identification function of your ID card is not activated. Please contact your responsible authority to activate the online identification function.") :
					//: INFO ANDROID IOS The ID card may be inserted, the authentication process may be started.
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
