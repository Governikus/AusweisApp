import QtQuick 2.5
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.TechnologyInfo 1.0

Item {
	id: baseItem
	signal requestPluginType(string pReaderPlugInType)

	property bool settingsPushed: remoteServiceSettings.visible
	property bool wifiEnabled: applicationModel.wifiEnabled
	property bool foundSelectedReader: applicationModel.foundSelectedReader

	Connections {
		target: applicationModel
		onFireCertificateRemoved: {
			qmlExtension.showFeedback(qsTr("The device %1 was unpaired because it does not react to connection attempts. Retry the pairing process if you want to use this device to authenticate yourself.").arg(pDeviceName))
		}
	}

	onFoundSelectedReaderChanged: {
		if (baseItem.settingsPushed && foundSelectedReader) {
			remoteServiceSettings.firePop()
		}
	}

	ProgressIndicator {
		id: progressIndicator
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		height: parent.height / 2
		imageIconSource: "qrc:///images/icon_remote.svg"
		imagePhoneSource: "qrc:///images/phone_remote.svg"
		state: foundSelectedReader ? "two" : "one"
	}

	TechnologyInfo {
		id: techInfo
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: switchToNfcAction.top
		state: parent.state

		enableButtonVisible: !wifiEnabled || !foundSelectedReader
		enableButtonText: {
			settingsModel.translationTrigger

			if (!wifiEnabled) {
				return qsTr("Enable Wifi");
			} else if (!foundSelectedReader) {
				return qsTr("Pair device");
			} else {
				return qsTr("Continue")
			}
		}

		onEnableClicked: {
			if (!wifiEnabled) {
				applicationModel.enableWifi()
			} else if (!baseItem.settingsPushed) {
				firePush(remoteServiceSettings, {})
			}
		}
		enableText: {
			settingsModel.translationTrigger

			if (!wifiEnabled) {
				return qsTr("To use the remote service WiFi has to be activated. Please activate WiFi in your device settings.");
			} else if (!foundSelectedReader) {
				return qsTr("No paired and activated remote device was detected. Make sure that you have started remote service on you remote device.");
			} else {
				return "";
			}
		}

		titleText: (foundSelectedReader ?
			qsTr("Determine card") :
			qsTr("Establish connection")
		) + settingsModel.translationTrigger

		subTitleText: {
			settingsModel.translationTrigger

			if (!visible) {
				return "";
			} else if (!!numberModel.inputError) {
				return numberModel.inputError;
			} else if (numberModel.extendedLengthApdusUnsupported) {
				qsTr("Your remote device does not meet the technical requirements (Extended Length not supported).");
			} else if (numberModel.pinDeactivated) {
				qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.");
			} else {
				return qsTr("Connected to %1. Please insert your ID card.").arg(remoteServiceModel.connectedServerDeviceNames);
			}
		}
	}

	TechnologySwitch {
		id: switchToNfcAction
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: "REMOTE"
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}

	RemoteServiceSettings {
		id: remoteServiceSettings
		visible: false
	}
}
