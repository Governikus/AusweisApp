/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.RemoteServiceView
import Governikus.TechnologyInfo
import Governikus.Type.ApplicationModel
import Governikus.Type.AuthModel
import Governikus.Type.RemoteServiceModel
import Governikus.Type.ReaderPlugIn
import Governikus.Type.NumberModel

GFlickableColumnLayout {
	id: baseItem

	property bool foundSelectedReader: ApplicationModel.availableReader > 0
	property bool wifiEnabled: ApplicationModel.wifiEnabled

	signal deviceUnpaired(var pDeviceName)

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	Connections {
		function onFireCertificateRemoved(pDeviceName) {
			deviceUnpaired(pDeviceName);
		}

		target: RemoteServiceModel
	}
	RemoteProgressIndicator {
		Accessible.ignored: true
		Layout.alignment: Qt.AlignHCenter
		foundSelectedReader: baseItem.foundSelectedReader
	}
	TechnologyInfo {
		id: techInfo

		Layout.alignment: Qt.AlignHCenter
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
		}

		onEnableClicked: {
			if (!wifiEnabled) {
				ApplicationModel.enableWifi();
			} else if (!foundSelectedReader) {
				push(remoteServiceSettings);
			}
		}
	}
	Component {
		id: remoteServiceSettings

		RemoteServiceSettings {
			onPairingSuccessful: pop()
		}
	}
}
