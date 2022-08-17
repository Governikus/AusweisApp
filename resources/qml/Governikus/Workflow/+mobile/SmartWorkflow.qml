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

	property var workflowModel
	readonly property int smartState: SmartModel.smartState
	readonly property bool isSmartCardAllowed: workflowModel.isSmartCardAllowed
	readonly property bool canUseSmart: smartState === SmartModel.SMART_READY && isSmartCardAllowed && SmartModel.isScanRunning

	SmartProgressIndicator {
		id: progressIndicator

		Accessible.ignored: true

		anchors {
			left: parent.left
			top: parent.top
			right: parent.right
		}
		height: parent.height / 2

		disabled: !canUseSmart
	}

	TechnologyInfo {
		id: technologyInfo

		anchors {
			left: parent.left
			right: parent.right
			top: progressIndicator.bottom
			bottom: parent.bottom
			leftMargin: Constants.component_spacing
			rightMargin: Constants.component_spacing
		}

		titleText: {
			switch (smartState) {
				case SmartModel.SMART_UPDATING_STATUS:
					//: LABEL ANDROID IOS
					return qsTr("Updating Smart-eID status...")

				case SmartModel.SMART_UNAVAILABLE:
					//: LABEL ANDROID IOS
					return qsTr("Smart-eID unsupported")

				case SmartModel.SMART_READY:
					if (!isSmartCardAllowed) {
						//: LABEL ANDROID IOS
						return qsTr("Smart-eID disallowed")
					}

					if (!SmartModel.isScanRunning) {
						//: LABEL ANDROID IOS
						return qsTr("Updating Smart-eID status...")
					}

					//: LABEL ANDROID IOS
					return qsTr("Smart-eID")

				default:
					//: LABEL ANDROID IOS
					return qsTr("Smart-eID not ready")
			}
		}

		subTitleText: {
			if (canUseSmart) {
				//: LABEL ANDROID IOS
				return qsTr("Your Smart-eID is ready for use, press \"Continue\" to proceed.")
			}

			return ""
		}

		enableText: {
			switch (smartState) {
				case SmartModel.SMART_UPDATING_STATUS:
					//: LABEL ANDROID IOS
					return qsTr("Please wait a moment.")

				case SmartModel.SMART_UNAVAILABLE:
					//: LABEL ANDROID IOS
					return qsTr("Unfortunately, Smart-eID is not supported by your device.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader.")

				case SmartModel.SMART_READY:
					if (!isSmartCardAllowed) {
						//: LABEL ANDROID IOS
						return qsTr("Unfortunately, using your Smart-eID for this authentication is not allowed by the provider.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader.")
					}

					if (!SmartModel.isScanRunning) {
						//: LABEL ANDROID IOS
						return qsTr("Please wait a moment.")
					}

					return ""

				default:
					//: LABEL ANDROID IOS
					return qsTr("You have not yet set up a Smart-eID or it is no longer usable.\n\nTo proceed use your ID card by selecting the NFC interface or choose \"WiFi\" to connect with another device as cardreader. If you want to set up a Smart-eID instead, please abort the current process and start the Smart-eID setup from the main screen.")
			}
		}

		enableButtonText: {
			if (canUseSmart) {
				return qsTr("Continue")
			}

			return ""
		}

		onEnableClicked: if (canUseSmart) workflowModel.insertSmartCard()
	}
}
