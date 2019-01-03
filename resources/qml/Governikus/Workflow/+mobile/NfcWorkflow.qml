import QtQuick 2.10
import QtQuick.Layouts 1.1

import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.Type.NumberModel 1.0


Item {
	id: baseItem
	signal requestPluginType(int pReaderPlugInType)
	clip: true

	Item {
		id: progressIndicatorWrapper
		height: 2 * baseItem.height / 5
		anchors.left: parent.left
		anchors.right: parent.right

		NfcProgressIndicator {
			id: progressIndicator
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: baseItem.height / 2
			state: visible && ApplicationModel.nfcAvailable && ApplicationModel.nfcEnabled ? "on" : "off"
		}
	}


	TechnologyInfo {
		id: technologyInfo

		anchors.left: parent.left
		anchors.leftMargin: Utils.dp(5)
		anchors.right: parent.right
		anchors.rightMargin: anchors.leftMargin
		anchors.top: progressIndicatorWrapper.bottom
		anchors.bottom: technologySwitch.top
		clip: true

		enableButtonVisible: ApplicationModel.nfcAvailable && !ApplicationModel.nfcEnabled
		enableButtonText: qsTr("Go to NFC settings") + settingsModel.translationTrigger
		onEnableClicked: qmlExtension.showSettings("android.settings.NFC_SETTINGS")
		enableText: (!visible ? "" :
					!ApplicationModel.nfcAvailable ? qsTr("NFC is not supported by your device.") + "<br/>" + qsTr("Please try Bluetooth.") :
					!ApplicationModel.nfcEnabled ? qsTr("NFC is switched off.") + "<br/>" + qsTr("Please enable NFC in your system settings.") : ""
					) + settingsModel.translationTrigger

		titleText: qsTr("Establish connection") + settingsModel.translationTrigger

		subTitleText: (!visible ? "" :
					  ApplicationModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). You require an additional 'Bluetooth card reader' or an additional 'smartphone as card reader' to use the online identification function with this device.") :
					  NumberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.") :
					  qsTr("Please place your device<br/>on your ID card.")
					  ) + settingsModel.translationTrigger
	}


	TechnologySwitch {
		id: technologySwitch
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		selectedTechnology: ReaderPlugIn.NFC
		onRequestPluginType: parent.requestPluginType(pReaderPlugInType)
	}
}
