import QtQuick 2.5
import QtQuick.Layouts 1.1

import "global"


Item {
	id: baseItem
	signal abortNfc
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
		anchors.bottom: switchToBluetoothAction.top
		clip: true
		state: parent.state

		enableButtonVisible: applicationModel.nfcAvailable && !applicationModel.nfcEnabled
		enableButtonText: qsTr("Enable NFC")
		onEnableClicked: qmlExtension.showSettings("android.settings.NFC_SETTINGS")
		enableText: !visible ? "" :
					!applicationModel.nfcAvailable ? qsTr("NFC is not supported by your device.<br/>Please try Bluetooth.") :
					!applicationModel.nfcEnabled ? qsTr("NFC is switched off.<br/>Please enable NFC.") : ""

		titleText: qsTr("Establish connection")

		subTitleText: !visible ? "" :
					  numberModel.extendedLengthApdusUnsupported ? qsTr("Your device does not meet the technical requirements (Extended Length not supported). You require an additional Bluetooth card reader to use the online identification function with this device.") :
					  numberModel.pinDeactivated ? qsTr("The online identification function of your ID card is deactivated. Please contact your competent authority to activate the online identification function.") :
					  qsTr("Please place your device<br/>on your ID card.")
		subTitleTextRedColor: numberModel.extendedLengthApdusUnsupported || numberModel.pinDeactivated
	}


	TechnologySwitchButton {
		id: switchToBluetoothAction
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		onClicked: parent.abortNfc()
		imageSource: "qrc:///images/icon_Bluetooth.svg"
		text: qsTr("Use Bluetooth card reader<br/>instead of NFC")
		enabled: parent.state === "nfc_connect" || parent.state === "nfc_card"
	}
}
