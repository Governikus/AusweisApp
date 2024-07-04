/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TechnologyInfo
import Governikus.Type

GFlickableColumnLayout {
	id: baseItem

	readonly property bool isRemoteWorkflow: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.REMOTE_SERVICE
	readonly property int nfcState: visible ? ApplicationModel.nfcState : ApplicationModel.NfcState.UNAVAILABLE

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
			case ApplicationModel.NfcState.READY:
				return "on";
			case ApplicationModel.NfcState.UNAVAILABLE:
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
			case ApplicationModel.NfcState.DISABLED:
				//: INFO ANDROID IOS
				return qsTr("Go to NFC settings");
			case ApplicationModel.NfcState.INACTIVE:
				//: LABEL ANDROID IOS
				return qsTr("Start NFC scan");
			default:
				return "";
			}
		}
		enableText: {
			switch (nfcState) {
			case ApplicationModel.NfcState.UNAVAILABLE:
				//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
				return qsTr("This device cannot be used to read your ID card.") + "<br/><br/>" +
				//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
				qsTr("If you want to use the online identification on this device anyway, you can connect another NFC capable smartphone as a card reader.") + "<br/><br/>" +
				//: INFO ANDROID IOS AA2 can't use NFC on this device, suggest to use SaK instead.
				qsTr("To pair a smartphone select the <b>WiFi</b> option and click <b>Manage Pairings</b>.");
			case ApplicationModel.NfcState.DISABLED:
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				return qsTr("NFC is switched off.") + "<br/>" +
				//: INFO ANDROID IOS NFC is available but needs to be activated in the settings of the smartphone.
				qsTr("Please enable NFC in your system settings.");
			case ApplicationModel.NfcState.INACTIVE:
				//: INFO ANDROID IOS NFC is available and enabled but needs to be started.
				return qsTr("NFC scan is not running.") + "<br/>" +
				//: INFO ANDROID IOS NFC is available and enabled but needs to be started.
				qsTr("Please start the NFC scan.");
			default:
				return "";
			}
		}
		subTitleText: {
			if (nfcState !== ApplicationModel.NfcState.READY) {
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
			case ApplicationModel.NfcState.UNAVAILABLE:
				//: INFO ANDROID IOS
				return qsTr("NFC is not available");
			case ApplicationModel.NfcState.DISABLED:
				//: INFO ANDROID IOS
				return qsTr("NFC is disabled");
			case ApplicationModel.NfcState.INACTIVE:
				//: INFO ANDROID IOS
				return qsTr("Start scan");
			default:
				//: INFO ANDROID IOS
				return qsTr("Establish connection");
			}
		}

		onEnableClicked: nfcState === ApplicationModel.NfcState.DISABLED ? ApplicationModel.showSettings(ApplicationModel.Settings.NFC) : startScanIfNecessary()
		onReceivedFocus: pItem => baseItem.positionViewAtItem(pItem)
	}
}
