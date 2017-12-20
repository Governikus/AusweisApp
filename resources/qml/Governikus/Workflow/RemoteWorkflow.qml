import QtQuick 2.5
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0

Item {
	id: baseItem
	signal requestPluginType(string pReaderPlugInType)

	ProgressIndicator {
		id: progressIndicator
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.right: parent.right
		height: parent.height / 2
		imageIconSource: "qrc:///images/icon_remote.svg"
		imagePhoneSource: "qrc:///images/phone_remote.svg"
		state: applicationModel.foundSelectedReader ? "two" : "one"
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

		enableButtonVisible: false
		enableButtonText: (!applicationModel.wifiEnabled ? qsTr("Enable Wifi") : qsTr("Continue")) + settingsModel.translationTrigger
		onEnableClicked: {
			// open wifi dialogue
		}

		titleText: (!applicationModel.foundSelectedReader ?
						qsTr("Establish connection") :
						qsTr("Determine card")
					) + settingsModel.translationTrigger

		subTitleText: (!visible ? "" :
					  !!numberModel.inputError ? numberModel.inputError :
					  !applicationModel.wifiEnabled ? qsTr("To use the remote service WiFi has to be activated. Please activate WiFi in your device settings.") :
					  !applicationModel.foundSelectedReader ?
							qsTr("No paired and activated remote device was detected. Make sure that you have started remote service on you remote device.") :
							qsTr("Please insert your ID card.")
					  ) + settingsModel.translationTrigger
		subTitleTextRedColor: !applicationModel.wifiEnabled
	}

	TechnologySwitch {
		id: switchToNfcAction
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: "REMOTE"
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}
}
