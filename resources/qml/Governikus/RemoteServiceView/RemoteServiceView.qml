/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


SectionPage {
	id: baseItem

	navigationAction: NavigationAction {
		state: RemoteServiceModel.running ? "cancel" : ""
		onClicked: RemoteServiceModel.running = false
	}

			//: LABEL ANDROID IOS
	title: sectionSwitch.selectedSection === "LOCAL" ? qsTr("Configure local settings") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			: sectionSwitch.selectedSection === "REMOTE" ? qsTr("Pair remote devices")+ SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			: qsTr("Remote service") + SettingsModel.translationTrigger

	Connections {
		target: RemoteServiceModel
		//: ERROR ANDROID IOS An error occurred while pairing the device.
		onFirePairingFailed: ApplicationModel.showFeedback(qsTr("Pairing failed. Please try again to activate pairing on your other device and enter the shown pairing code."))
	}

	onVisibleChanged: {
		if (visible && sectionSwitch.selectedSection === "REMOTE" && !RemoteServiceModel.detectRemoteDevices) {
			RemoteServiceModel.detectRemoteDevices = true
		}
		if (!visible && RemoteServiceModel.detectRemoteDevices && !remoteSettingsView.pinEntryInProgress) {
			RemoteServiceModel.detectRemoteDevices = false
		}
	}

	content: ColumnLayout {
		height: baseItem.height

		RemoteServiceViewStartStop {
			visible: sectionSwitch.selectedSection == SectionSwitch.Section.STARTSTOP
			width: baseItem.width
			Layout.fillHeight: true
		}
		GFlickable {
			id: flickable

			visible: sectionSwitch.selectedSection == SectionSwitch.Section.REMOTE
			width: baseItem.width
			Layout.fillHeight: true

			contentHeight: remoteSettingsView.height
			RemoteServiceViewRemote {
				id: remoteSettingsView
				width: baseItem.width
				parentSectionPage: baseItem
			}
		}
		RemoteServiceViewLocal {
			visible: sectionSwitch.selectedSection == SectionSwitch.Section.LOCAL
			width: baseItem.width
			Layout.fillHeight: true
		}

		SectionSwitch {
			id: sectionSwitch

			visible: !RemoteServiceModel.running
			Layout.fillWidth: true

			onSelectedSectionChanged: {
				if (selectedSection == SectionSwitch.Section.REMOTE) {
					RemoteServiceModel.detectRemoteDevices = true
					flickable.highlightScrollbar()
				} else {
					RemoteServiceModel.detectRemoteDevices = false
				}
			}
		}
	}

	RemoteServiceController {
		id: controller
	}
}
