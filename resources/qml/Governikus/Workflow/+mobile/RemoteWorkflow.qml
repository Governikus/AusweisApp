import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem
	signal requestPluginType(int pReaderPlugInType)

	property bool settingsPushed: remoteServiceSettings.visible
	property bool wifiEnabled: ApplicationModel.wifiEnabled
	property bool foundSelectedReader: ApplicationModel.foundSelectedReader

	Connections {
		target: ApplicationModel
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
		state: foundSelectedReader ? "two" : "off"
	}

	TechnologyInfo {
		id: techInfo
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: switchToNfcAction.top

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
				ApplicationModel.enableWifi()
			} else if (!baseItem.settingsPushed) {
				firePush(remoteServiceSettings)
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
			} else if (!!NumberModel.inputError) {
				return NumberModel.inputError;
			} else if (ApplicationModel.extendedLengthApdusUnsupported) {
				qsTr("Your remote device does not meet the technical requirements (Extended Length not supported).");
			} else if (NumberModel.pinDeactivated) {
				return qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.");
			} else {
				return qsTr("Connected to %1. Please insert your ID card.").arg(RemoteServiceModel.connectedServerDeviceNames);
			}
		}
	}

	TechnologySwitch {
		id: switchToNfcAction
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: ReaderPlugIn.REMOTE
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}

	RemoteServiceSettings {
		id: remoteServiceSettings
		visible: false
	}
}
