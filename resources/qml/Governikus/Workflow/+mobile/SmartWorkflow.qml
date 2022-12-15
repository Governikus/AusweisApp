/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.TechnologyInfo 1.0
import Governikus.Type.SmartModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.PersonalizationModel 1.0

Item {
	id: baseItem

	readonly property bool canUseSmart: smartState === SmartModel.SMART_READY && isSmartCardAllowed && SmartModel.isScanRunning
	readonly property bool isSmartCardAllowed: workflowModel.isSmartCardAllowed
	readonly property int smartState: SmartModel.smartState
	property var workflowModel

	SmartProgressIndicator {
		id: progressIndicator
		Accessible.ignored: true
		disabled: !canUseSmart
		height: parent.height / 2

		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
		}
	}
	TechnologyInfo {
		id: technologyInfo
		enableButtonText: {
			if (canUseSmart) {
				return qsTr("Continue");
			}
			return "";
		}
		enableText: {
			switch (smartState) {
			case SmartModel.SMART_UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Please wait a moment.");
			case SmartModel.SMART_UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Unfortunately, Smart-eID is not supported by your device.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader.");
			case SmartModel.SMART_READY:
				if (!isSmartCardAllowed) {
					//: LABEL ANDROID IOS
					return qsTr("Unfortunately, using your Smart-eID for this authentication is not allowed by the provider.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader.");
				}
				if (!SmartModel.isScanRunning) {
					//: LABEL ANDROID IOS
					return qsTr("Please wait a moment.");
				}
				return "";
			default:
				//: LABEL ANDROID IOS
				return qsTr("You have not yet set up a Smart-eID or it is no longer usable.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader. If you want to set up a Smart-eID instead, please abort the current process and start the Smart-eID setup from the main screen.");
			}
		}
		subTitleText: {
			if (canUseSmart) {
				//: LABEL ANDROID IOS
				return qsTr("Your Smart-eID is ready for use, press \"Continue\" to proceed.");
			}
			return "";
		}
		titleText: {
			switch (smartState) {
			case SmartModel.SMART_UPDATING_STATUS:
				//: LABEL ANDROID IOS
				return qsTr("Updating Smart-eID status...");
			case SmartModel.SMART_UNAVAILABLE:
				//: LABEL ANDROID IOS
				return qsTr("Smart-eID unsupported");
			case SmartModel.SMART_READY:
				if (!isSmartCardAllowed) {
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

		onEnableClicked: if (canUseSmart)
			workflowModel.insertSmartCard()

		anchors {
			bottom: parent.bottom
			left: parent.left
			leftMargin: Constants.component_spacing
			right: parent.right
			rightMargin: Constants.component_spacing
			top: progressIndicator.bottom
		}
	}
}
