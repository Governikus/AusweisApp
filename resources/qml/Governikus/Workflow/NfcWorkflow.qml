import QtQuick 2.5
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0

Item {
	id: baseItem
	signal requestPluginType(string pReaderPlugInType)
	clip: true

	NfcProgressIndicator {
		id: progressIndicator
		anchors.left: parent.left
		anchors.right: parent.right
		height: parent.height / 2
		state: visible && applicationModel.nfcAvailable && applicationModel.nfcEnabled ? "on" : "off"
	}


	TechnologyInfo {
		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicator.bottom
		anchors.bottom: technologySwitch.top
		clip: true
		state: parent.state

		enableButtonVisible: applicationModel.nfcAvailable && !applicationModel.nfcEnabled
		enableButtonText: qsTr("Go to NFC settings") + settingsModel.translationTrigger
		onEnableClicked: qmlExtension.showSettings("android.settings.NFC_SETTINGS")
		enableText: (!visible ? "" :
					!applicationModel.nfcAvailable ? qsTr("NFC is not supported by your device.") + "<br/>" + qsTr("Please try Bluetooth.") :
					!applicationModel.nfcEnabled ? qsTr("NFC is switched off.") + "<br/>" + qsTr("Please enable NFC in your system settings.") : ""
					) + settingsModel.translationTrigger

		titleText: qsTr("Establish connection") + settingsModel.translationTrigger

		subTitleText: (!visible ? "" :
					  numberModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). You require an additional 'Bluetooth card reader' or an additional 'smartphone as card reader' to use the online identification function with this device.") :
					  numberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.") :
					  qsTr("Please place your device<br/>on your ID card.")
					  ) + settingsModel.translationTrigger
	}


	TechnologySwitch {
		id: technologySwitch
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: "NFC"
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}
}
