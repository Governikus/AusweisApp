/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0

Item {
	id: baseItem

	readonly property int nfcState: visible ? ApplicationModel.nfcState : ApplicationModel.NFC_UNAVAILABLE

	signal startScanIfNecessary

	NfcProgressIndicator {
		id: progressIndicator
		Accessible.ignored: true
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		height: baseItem.height / 2
		state: nfcState === ApplicationModel.NFC_READY ? "on" : "off"
	}
	TechnologyInfo {
		id: technologyInfo
		enableButtonText: {
			switch (nfcState) {
			case ApplicationModel.NFC_DISABLED:
				//: INFO ANDROID IOS
				return qsTr("Go to NFC settings");
			case ApplicationModel.NFC_INACTIVE:
				//: LABEL ANDROID IOS
				return qsTr("Start NFC scan");
			default:
				return "";
			}
		}
		enableText: {
			switch (nfcState) {
			case ApplicationModel.NFC_UNAVAILABLE:
				//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
				return qsTr("Unfortunately, this functionality is not available on your device.") + "<br/>" +
				//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
				qsTr("However, you can use a separate smartphone as card reader to utilize the eID function.");
			case ApplicationModel.NFC_DISABLED:
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("NFC is switched off.") + "<br/>" +
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				qsTr("Please enable NFC in your system settings.");
			case ApplicationModel.NFC_INACTIVE:
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("NFC scan is not running.") + "<br/>" +
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				qsTr("Please start the NFC scan.");
			default:
				return "";
			}
		}
		subTitleText: {
			if (nfcState !== ApplicationModel.NFC_READY) {
				return "";
			}
			if (ApplicationModel.extendedLengthApdusUnsupported) {
				//: INFO ANDROID IOS The NFC interface does not meet the minimum requirements, using a different smartphone is suggested.
				return qsTr("Your device does not meet the technical requirements (Extended Length not supported). However you can use a separate smartphone as card reader to utilize the eID function.");
			} else if (Constants.is_layout_ios) {
				//: INFO IOS The ID card may be inserted, the authentication process may be started.
				return qsTr("Please place your ID card on the top of the device's back side.");
			} else {
				//: INFO ANDROID The ID card may be inserted, the authentication process may be started.
				return qsTr("Please place your ID card directly on the device's back side.<br/><br/>The exact position of the ID card is device dependent. The animations depict possible positions. Keep one position for several seconds before trying another one and do not move the ID card after contact was established.");
			}
		}
		titleText: {
			switch (nfcState) {
			case ApplicationModel.NFC_UNAVAILABLE:
				//: INFO ANDROID IOS
				return qsTr("NFC is not available");
			case ApplicationModel.NFC_DISABLED:
				//: INFO ANDROID IOS
				return qsTr("NFC is disabled");
			case ApplicationModel.NFC_INACTIVE:
				//: INFO ANDROID IOS
				return qsTr("Start scan");
			default:
				//: INFO ANDROID IOS
				return qsTr("Establish connection");
			}
		}

		onEnableClicked: nfcState === ApplicationModel.NFC_DISABLED ? ApplicationModel.showSettings(ApplicationModel.SETTING_NFC) : startScanIfNecessary()

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: Constants.component_spacing
			right: parent.right
			rightMargin: Constants.component_spacing
			top: progressIndicator.bottom
		}
	}
}
