/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


Item {
	id: baseItem
	height: mainColumn.height

	property var parentSectionPage: undefined

	property bool pinEntryInProgress: false

	Column {
		id: mainColumn
		width: baseItem.width
		padding: Constants.pane_padding
		spacing: Constants.component_spacing

		readonly property int usableWidth: width - 2 * padding
		Column {
			width: parent.usableWidth

			GText {
				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: LABEL ANDROID IOS
				text: qsTr("Paired devices") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_accent
				bottomPadding: Constants.groupbox_spacing
			}

			GText {
				width: parent.width
				visible: !knownDeviceList.visible

				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: LABEL ANDROID IOS
				text: qsTr("No device is paired.") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_secondary
			}

			ListView {
				id: knownDeviceList
				width: parent.width
				height: childrenRect.height
				spacing: Constants.groupbox_spacing / 2
				model: RemoteServiceModel.knownDevices
				delegate: KnownDevicesListDelegate {
					width: knownDeviceList.width
				}
				visible: count > 0
				interactive: false
			}
		}

		Column {
			width: parent.usableWidth

			GText {
				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: LABEL ANDROID IOS
				text: qsTr("Available devices") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_accent
				bottomPadding: Constants.groupbox_spacing
			}

			GText {
				width: parent.width
				visible: !searchDeviceList.visible

				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: INFO ANDROID IOS No remote reader was found on the network, both devices need to be connected to the same wifi network.
				text: qsTr("No new remote reader was found on your network. Make sure that the remote reader functionality in AusweisApp2 on your other device is activated and that your devices are connected to the same network.") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_secondary
			}

			ListView {
				id: searchDeviceList
				width: parent.width
				height: childrenRect.height
				model: RemoteServiceModel.availableRemoteDevices
				spacing: Constants.groupbox_spacing / 2
				delegate: AvailableDevicesListDelegate {
					width: searchDeviceList.width
					onRequestPairing: {
						if (!RemoteServiceModel.rememberServer(pDeviceId)) {
							return
						}
						informationPairingPopup.open()
					}
				}
				visible: count > 0
				interactive: false
			}
		}
	}

	ConfirmationPopup {
		id: informationPairingPopup

		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO ANDROID IOS
		title: qsTr("Pairing mode") + SettingsModel.translationTrigger
		//: INFO ANDROID IOS Information dialog that requests the user to start the pairing mode on the smarthpone.
		text: qsTr("Please start pairing mode first.") + SettingsModel.translationTrigger

		onConfirmed: {
			pinEntryInProgress = true
			firePush(enterPinView)
		}
	}

	EnterPasswordView {
		id: enterPinView
		state: "REMOTE_PIN"
		navigationAction: NavigationAction { state: "cancel"; onClicked: { firePop(); pinEntryInProgress = false } }
		//: LABEL ANDROID IOS
		title: qsTr("Pairing code") + SettingsModel.translationTrigger
		visible: false

		onPasswordEntered: {
			firePop()
			pinEntryInProgress = false
		}
	}
}
