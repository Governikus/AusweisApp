/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem

	signal startScanIfNecessary()


	NfcProgressIndicator {
		id: progressIndicator

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: baseItem.height / 2

		state: visible && ApplicationModel.nfcAvailable && ApplicationModel.nfcEnabled ? "on" : "off"
	}

	TechnologyInfo {
		id: technologyInfo

		anchors {
			left: parent.left
			right: parent.right
			top: progressIndicator.bottom
			bottom: parent.bottom
			leftMargin: Constants.component_spacing
			rightMargin: Constants.component_spacing
		}

		enableButtonVisible: ApplicationModel.nfcAvailable && (!ApplicationModel.nfcEnabled || !ApplicationModel.nfcRunning)
		enableButtonText: (!ApplicationModel.nfcEnabled ?
						   //: LABEL ANDROID IOS
						   qsTr("Go to NFC settings") :
						   //: LABEL ANDROID IOS
						   qsTr("Start NFC scan")
						  )
		onEnableClicked: !ApplicationModel.nfcEnabled ? ApplicationModel.showSettings(ApplicationModel.SETTING_NFC) : startScanIfNecessary()
		enableText: (!visible ? "" :
					!ApplicationModel.nfcAvailable ?
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
					qsTr("Unfortunately, this functionality is not available on your device.") + "<br/>" +
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
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
					)

		titleText: qsTr("Establish connection")

		subTitleText: (!visible ? "" :
					//: INFO ANDROID IOS The NFC interface does not meet the minimum requirements, using a different smartphone is suggested.
					  ApplicationModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). However you can use a separate 'smartphone as card reader' to utilize the online identification function.") :
					//: INFO ANDROID IOS The online authentication feature is disabled and needs to be activated by the authorities.
					  NumberModel.pinDeactivated ? qsTr("The online identification function of your ID card is not activated. Please contact your responsible authority to activate the online identification function.") :
					  Constants.is_layout_ios ?
					  //: INFO IOS The ID card may be inserted, the authentication process may be started.
					  qsTr("Please place your ID card on the top of the device's back side.") :
					  //: INFO ANDROID The ID card may be inserted, the authentication process may be started.
					  qsTr("Please place your ID card on the device, the exact position is device dependent. The animations depict possible positions. Keep one position for several seconds before trying another one and do not move the ID card after contact was established.")
					  )
	}
}
