/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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

	readonly property int nfcState: visible ? ApplicationModel.nfcState : ApplicationModel.NFC_UNAVAILABLE

	signal startScanIfNecessary()

	NfcProgressIndicator {
		id: progressIndicator

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: baseItem.height / 2

		state: nfcState === ApplicationModel.NFC_READY ? "on" : "off"
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

		enableButtonVisible: nfcState === ApplicationModel.NFC_DISABLED || nfcState === ApplicationModel.NFC_INACTIVE
		enableButtonText: (nfcState === ApplicationModel.NFC_DISABLED ?
						   //: LABEL ANDROID IOS
						   qsTr("Go to NFC settings") :
						   //: LABEL ANDROID IOS
						   qsTr("Start NFC scan")
						  )
		onEnableClicked: nfcState === ApplicationModel.NFC_DISABLED ? ApplicationModel.showSettings(ApplicationModel.SETTING_NFC) : startScanIfNecessary()
		enableText: {
			switch (nfcState) {
				case ApplicationModel.NFC_UNAVAILABLE:
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
					return qsTr("Unfortunately, this functionality is not available on your device.") + "<br/>" +
					//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
					qsTr("However, you can use a separate smartphone as card reader to utilize the online identification function.")
				case ApplicationModel.NFC_DISABLED:
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					return qsTr("NFC is switched off.") + "<br/>" +
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					qsTr("Please enable NFC in your system settings.")
				case ApplicationModel.NFC_INACTIVE:
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					return qsTr("NFC scan is not running.") + "<br/>" +
					//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
					qsTr("Please start the NFC scan.")
				default:
					return ""
			}
		}
		titleText: qsTr("Establish connection")

		subTitleText: (!visible ? "" :
					  //: INFO ANDROID IOS The NFC interface does not meet the minimum requirements, using a different smartphone is suggested.
					  ApplicationModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). However you can use a separate smartphone as card reader to utilize the online identification function.") :
					  Constants.is_layout_ios ?
					  //: INFO IOS The ID card may be inserted, the authentication process may be started.
					  qsTr("Please place your ID card on the top of the device's back side.") :
					  //: INFO ANDROID The ID card may be inserted, the authentication process may be started.
					  qsTr("Please place your ID card on the device, the exact position is device dependent. The animations depict possible positions. Keep one position for several seconds before trying another one and do not move the ID card after contact was established.")
					  )
	}
}
