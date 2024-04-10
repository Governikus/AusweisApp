/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TechnologyInfo
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.ReaderPlugIn
import Governikus.Type.NumberModel
import Governikus.Type.RemoteServiceModel
import Governikus.Type.SettingsModel

GFlickableColumnLayout {
	id: baseItem

	readonly property bool isRemoteWorkflow: ApplicationModel.currentWorkflow === ApplicationModel.WORKFLOW_REMOTE_SERVICE
	readonly property int nfcState: visible ? ApplicationModel.nfcState : ApplicationModel.NFC_UNAVAILABLE

	signal startScanIfNecessary

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	NfcProgressIndicator {
		id: progressIndicator

		Accessible.ignored: true
		Layout.alignment: Qt.AlignCenter
		state: {
			switch (nfcState) {
			case ApplicationModel.NFC_READY:
				return "on";
			case ApplicationModel.NFC_UNAVAILABLE:
				return "unavailable";
			default:
				return "off";
			}
		}
	}
	TechnologyInfo {
		id: technologyInfo

		//: INFO ANDROID Text regarding ID card position animations
		readonly property string animationText: qsTr("The animations depict possible positions.")
		//: INFO ANDROID Text regarding different NFC chip positions on different smartphones
		readonly property string deviceText: qsTr("The exact position of the ID card is device dependent.")
		//: INFO ANDROID Text that one ID card position should be kept for several seconds
		readonly property string keepPositionText: qsTr("Keep one position for several seconds before trying another one and do not move the ID card after contact was established.")
		//: INFO ANDROID Text regarding the ID card position on the device
		readonly property string positionText: qsTr("Please place your ID card directly on the device's back side.")

		Layout.alignment: Qt.AlignHCenter
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
				//: INFO ANDROID IOS NFC is available and enabled but needs to be started.
				return qsTr("NFC scan is not running.") + "<br/>" +
				//: INFO ANDROID IOS NFC is available and enabled but needs to be started.
				qsTr("Please start the NFC scan.");
			default:
				return "";
			}
		}
		subTitleText: {
			if (nfcState !== ApplicationModel.NFC_READY) {
				return "";
			}
			if (AuthModel.eidTypeMismatchError !== "") {
				return AuthModel.eidTypeMismatchError;
			}
			if (ApplicationModel.extendedLengthApdusUnsupported) {
				//: INFO ANDROID IOS The NFC interface does not meet the minimum requirements, using a different smartphone is suggested.
				return qsTr("Your device does not meet the technical requirements (Extended Length not supported). However you can use a separate smartphone as card reader to utilize the eID function.");
			} else if (Constants.is_layout_ios) {
				//: INFO IOS The ID card may be inserted, the authentication process may be started.
				return qsTr("Please place your ID card on the top of the device's back side.");
			} else if (SettingsModel.useAnimations) {
				return "%1<br/><br/>%2 %3 %4".arg(positionText).arg(deviceText).arg(animationText).arg(keepPositionText);
			} else {
				return "%1<br/><br/>%2 %3".arg(positionText).arg(deviceText).arg(keepPositionText);
			}
		}
		titleText: {
			if (isRemoteWorkflow && RemoteServiceModel.connectedClientName !== "") {
				//: INFO ANDROID IOS %1 will be replaced with the name of the device.
				return qsTr("The device \"%1\" wants to use this smartphone as card reader and connect to your id card.").arg(RemoteServiceModel.connectedClientName);
			}
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
	}
}
