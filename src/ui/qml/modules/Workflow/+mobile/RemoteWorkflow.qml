/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.RemoteServiceView
import Governikus.Type

GFlickableColumnLayout {
	id: root

	property bool foundSelectedReader: ApplicationModel.availableReader > 0
	property bool wifiEnabled: ApplicationModel.wifiEnabled

	signal deviceUnpaired(var pDeviceName)
	signal showRemoteServiceSettings

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			root.deviceUnpaired(pDeviceName);
		}

		target: RemoteServiceModel
	}
	TechnologyInfo {
		id: techInfo

		Layout.alignment: Qt.AlignHCenter
		enableButtonText: {
			if (!root.wifiEnabled) {
				//: LABEL ANDROID IOS
				return qsTr("Enable WiFi");
			} else if (!root.foundSelectedReader) {
				//: LABEL ANDROID IOS
				return qsTr("Manage pairings");
			} else {
				return "";
			}
		}
		infoText: {
			if (!root.wifiEnabled) {
				//: INFO ANDROID IOS The WiFi module needs to be enabled in the system settings to use the remote service.
				return qsTr("To use the remote service WiFi has to be activated. Please activate WiFi in your device settings.");
			} else if (!root.foundSelectedReader) {
				//: INFO ANDROID IOS No paired and reachable device was found, hint that the remote device needs to be actually started for this feature.
				return qsTr("Allow a connection on a paired smartphone or pair a new smartphone.");
			} else {
				return "";
			}
		}
		showAdditionalContent: RemoteServiceModel.requiresLocalNetworkPermission && root.wifiEnabled && !root.foundSelectedReader
		subTitleText: {
			if (!root.wifiEnabled || !root.foundSelectedReader) {
				return "";
			}
			if (!!NumberModel.inputError) {
				return NumberModel.inputError;
			} else if (!!AuthModel.eidTypeMismatchError) {
				return AuthModel.eidTypeMismatchError;
			} else if (ApplicationModel.extendedLengthApdusUnsupported) {
				//: INFO ANDROID IOS The device does not support Extended Length and can not be used as card reader.
				return qsTr("The connected smartphone as card reader (SaC) unfortunately does not meet the technical requirements (Extended Length not supported).");
			} else {
				//: INFO ANDROID IOS The connection to the smartphone was established, the ID card may be inserted.
				return qsTr("Connected to %1. Please follow the instructions on the connected smartphone.").arg(RemoteServiceModel.connectedServerDeviceNames);
			}
		}
		titleText: {
			if (!root.wifiEnabled) {
				//: LABEL ANDROID IOS
				return qsTr("Wifi disabled");
			} else if (!root.foundSelectedReader) {
				//: LABEL ANDROID IOS
				return qsTr("No smartphone as card reader connected");
			} else {
				//: LABEL ANDROID IOS
				return qsTr("Determine card");
			}
		}

		progressIndicator: RemoteProgressIndicator {
			Accessible.ignored: true
			foundSelectedReader: root.foundSelectedReader
		}

		onEnableClicked: {
			if (!root.wifiEnabled) {
				ApplicationModel.enableWifi();
			} else if (!root.foundSelectedReader) {
				root.showRemoteServiceSettings();
			}
		}

		LocalNetworkInfo {
		}
	}
}
