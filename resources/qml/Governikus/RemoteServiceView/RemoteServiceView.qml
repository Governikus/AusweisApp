import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.RemoteServiceModel 1.0

SectionPage {
	id: baseItem
	leftTitleBarAction: Constants.is_layout_ios ? iosTitleBarAction : androidTitleBarAction
	headerTitleBarAction: TitleBarAction {
		text: sectionSwitch.selectedSection === "LOCAL" ? qsTr("Configure local settings") + settingsModel.translationTrigger
			: sectionSwitch.selectedSection === "REMOTE" ? qsTr("Pair remote devices")+ settingsModel.translationTrigger
			: qsTr("Remote service") + settingsModel.translationTrigger
		font.bold: true }

	property var iosTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	property var androidTitleBarAction: TitleBarAction {
		state: RemoteServiceModel.running ? "cancel" : ""
		onClicked: RemoteServiceModel.running = !RemoteServiceModel.running
	}

	Connections {
		target: RemoteServiceModel
		onFirePairingFailed: qmlExtension.showFeedback(qsTr("Pairing failed. Please try again to activate pairing on your other device and enter the shown pairing code."))
	}

	onVisibleChanged: {
		if (visible && sectionSwitch.selectedSection === "REMOTE" && !RemoteServiceModel.detectRemoteDevices) {
			RemoteServiceModel.detectRemoteDevices = true
		}
		if (!visible && RemoteServiceModel.detectRemoteDevices && !remoteSettingsView.pinEntryInProgress) {
			RemoteServiceModel.detectRemoteDevices = false
		}
	}

	content: Column {
		width: baseItem.width

		RemoteServiceViewLocal
		{
			width: parent.width
			visible: sectionSwitch.selectedSection === "LOCAL"
		}

		RemoteServiceViewRemote
		{
			id: remoteSettingsView
			width: parent.width
			visible: sectionSwitch.selectedSection === "REMOTE"
			parentSectionPage: baseItem
		}

		RemoteServiceViewStartStop
		{
			width: parent.width
			height: baseItem.height
			visible: Constants.is_layout_android && sectionSwitch.selectedSection === "STARTSTOP"
		}
	}

	SectionSwitch {
		id: sectionSwitch
		visible: !RemoteServiceModel.running
		showStartStopButton: Constants.is_layout_android
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

		onSelectedSectionChanged: {
			if (selectedSection === "REMOTE" && !RemoteServiceModel.detectRemoteDevices){
				RemoteServiceModel.detectRemoteDevices = true
			} else {
				RemoteServiceModel.detectRemoteDevices = false
			}
		}
	}

	RemoteServiceController {
		id: controller
	}
}
