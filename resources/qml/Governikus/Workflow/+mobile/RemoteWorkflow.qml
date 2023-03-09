/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0

Item {
	id: baseItem

	property bool foundSelectedReader: ApplicationModel.availableReader > 0
	property bool settingsPushed: remoteServiceSettings.visible
	property bool wifiEnabled: ApplicationModel.wifiEnabled

	onFoundSelectedReaderChanged: {
		if (baseItem.settingsPushed && foundSelectedReader) {
			remoteServiceSettings.pop();
		}
	}

	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			//: INFO ANDROID IOS The paired smartphone was removed since it did not respond to connection attempts. It needs to be paired again before using it.
			ApplicationModel.showFeedback(qsTr("The device %1 was unpaired because it did not react to connection attempts. Pair the device again to use it as a card reader.").arg(pDeviceName));
		}

		target: RemoteServiceModel
	}
	ProgressIndicator {
		id: progressIndicator
		Accessible.ignored: true
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		height: parent.height / 2
		imageIconSource: "qrc:///images/mobile/icon_remote.svg"
		imagePhoneSource: "qrc:///images/mobile/phone_remote.svg"
		state: foundSelectedReader ? "two" : "off"
	}
	TechnologyInfo {
		id: techInfo
		enableButtonText: {
			if (!wifiEnabled) {
				//: LABEL ANDROID IOS
				return qsTr("Enable WiFi");
			} else if (!foundSelectedReader) {
				//: LABEL ANDROID IOS
				return qsTr("Pair device");
			} else {
				return "";
			}
		}
		enableText: {
			if (!wifiEnabled) {
				//: INFO ANDROID IOS The WiFi module needs to be enabled in the system settings to use the remote service.
				return qsTr("To use the remote service WiFi has to be activated. Please activate WiFi in your device settings.");
			} else if (!foundSelectedReader) {
				//: INFO ANDROID IOS No paired and reachable device was found, hint that the remote device needs to be actually started for this feature.
				return qsTr("No paired smartphone as card reader (SaC) with activated \"remote service\" available.");
			} else {
				return "";
			}
		}
		subTitleText: {
			if (!wifiEnabled || !foundSelectedReader) {
				return "";
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError;
			} else if (ApplicationModel.extendedLengthApdusUnsupported) {
				//: INFO ANDROID IOS The device does not support Extended Length and can not be used as card reader.
				qsTr("The connected smartphone as card reader (SaC) unfortunately does not meet the technical requirements (Extended Length not supported).");
			} else {
				//: INFO ANDROID IOS The connection to the smartphone was established, the ID card may be inserted.
				return qsTr("Connected to %1. Please place the NFC interface of the smartphone on your ID card.").arg(RemoteServiceModel.connectedServerDeviceNames);
			}
		}
		titleText: {
			if (!wifiEnabled) {
				//: LABEL ANDROID IOS
				return qsTr("Wifi disabled");
			} else if (!foundSelectedReader) {
				//: LABEL ANDROID IOS
				return qsTr("Waiting for connection");
			} else {
				//: LABEL ANDROID IOS
				return qsTr("Determine card");
			}
		}

		additionalContent: LocalNetworkInfo {
			visible: RemoteServiceModel.requiresLocalNetworkPermission && baseItem.wifiEnabled && !baseItem.foundSelectedReader
			width: parent.width
		}

		onEnableClicked: {
			if (!wifiEnabled) {
				ApplicationModel.enableWifi();
			} else if (!baseItem.settingsPushed) {
				push(remoteServiceSettings);
			}
		}

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: Constants.component_spacing
			right: parent.right
			rightMargin: Constants.component_spacing
			top: progressIndicator.bottom
		}
	}
	RemoteServiceSettings {
		id: remoteServiceSettings
		visible: false
	}
}
