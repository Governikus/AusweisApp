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

	signal deviceUnpaired(var pDeviceName)

	onFoundSelectedReaderChanged: {
		if (baseItem.settingsPushed && foundSelectedReader) {
			remoteServiceSettings.pop();
		}
	}

	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			deviceUnpaired(pDeviceName);
		}

		target: RemoteServiceModel
	}
	RemoteProgressIndicator {
		id: progressIndicator
		Accessible.ignored: true
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		foundSelectedReader: baseItem.foundSelectedReader
	}
	TechnologyInfo {
		id: techInfo
		enableButtonText: {
			if (!wifiEnabled) {
				//: LABEL ANDROID IOS
				return qsTr("Enable WiFi");
			} else if (!foundSelectedReader) {
				//: LABEL ANDROID IOS
				return qsTr("Manage pairings");
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
				return qsTr("Allow a connection on a paired smartphone or pair a new smartphone.");
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
				return qsTr("No smartphone as card reader connected");
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
