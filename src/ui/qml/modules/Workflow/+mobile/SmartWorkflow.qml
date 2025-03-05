/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type

GFlickableColumnLayout {
	id: root

	property bool autoInsertCard: false
	readonly property bool canUseSmart: smartState === SmartModel.State.READY && isSmartCardAllowed && SmartModel.isScanRunning
	readonly property bool isRemoteWorkflow: ApplicationModel.currentWorkflow === ApplicationModel.Workflow.REMOTE_SERVICE
	readonly property bool isSmartCardAllowed: workflowModel.isCurrentSmartCardAllowed
	readonly property int smartState: SmartModel.state
	property var workflowModel

	clip: true
	maximumContentWidth: Style.dimens.max_text_width
	spacing: 0
	topMargin: 0

	onCanUseSmartChanged: if (autoInsertCard)
		technologyInfo.enableClicked()

	TechnologyInfo {
		id: technologyInfo

		Layout.alignment: Qt.AlignHCenter
		enableButtonText: {
			if (root.canUseSmart && !root.autoInsertCard) {
				return qsTr("Continue");
			}
			return "";
		}
		infoText: {
			switch (root.smartState) {
			case SmartModel.State.UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Please wait a moment.");
			case SmartModel.State.UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Unfortunately, Smart-eID is not supported by your device.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader.");
			case SmartModel.State.READY:
				if (!root.isSmartCardAllowed) {
					//: LABEL ANDROID IOS
					return qsTr("Unfortunately, using your Smart-eID for this authentication is not allowed by the provider.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader.");
				}
				if (!SmartModel.isScanRunning) {
					//: LABEL ANDROID IOS
					return qsTr("Please wait a moment.");
				}
				return "";
			default:
				if (root.isRemoteWorkflow) {
					//: LABEL ANDROID IOS
					return qsTr("You have not yet set up a Smart-eID or it is no longer usable.\n\nTo proceed use your ID card by selecting the NFC interface. If you want to set up a Smart-eID instead, please abort the current process and start the Smart-eID setup from the main screen.");
				}
				//: LABEL ANDROID IOS
				return qsTr("You have not yet set up a Smart-eID or it is no longer usable.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader. If you want to set up a Smart-eID instead, please abort the current process and start the Smart-eID setup from the main screen.");
			}
		}
		subTitleText: {
			if (root.canUseSmart && !root.autoInsertCard) {
				//: LABEL ANDROID IOS
				return qsTr("Your Smart-eID is ready for use, press \"Continue\" to proceed.");
			}
			return "";
		}
		titleText: {
			if (root.isRemoteWorkflow && RemoteServiceModel.connectedClientName !== "") {
				//: INFO ANDROID IOS %1 will be replaced with the name of the device.
				return qsTr("The device \"%1\" wants to access your Smart-eID.").arg(RemoteServiceModel.connectedClientName);
			}
			switch (root.smartState) {
			case SmartModel.State.UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Updating Smart-eID status...");
			case SmartModel.State.UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID unsupported");
			case SmartModel.State.READY:
				if (!root.isSmartCardAllowed) {
					//: LABEL ANDROID IOS
					return qsTr("Smart-eID disallowed");
				}
				if (!SmartModel.isScanRunning) {
					//: LABEL ANDROID IOS
					return qsTr("Updating Smart-eID status...");
				}

				//: LABEL ANDROID IOS
				return qsTr("Smart-eID");
			default:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID not ready");
			}
		}

		progressIndicator: SmartProgressIndicator {
			Accessible.ignored: true
			disabled: !root.canUseSmart
		}

		onEnableClicked: if (root.canUseSmart)
			root.workflowModel.insertSmartCard()
	}
}
