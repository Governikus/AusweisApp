/*
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


Item {
	id: baseItem
	height: mainColumn.height

	Column {
		id: mainColumn
		width: baseItem.width
		leftPadding: Constants.pane_padding
		rightPadding: Constants.pane_padding
		spacing: Constants.component_spacing

		readonly property int usableWidth: width - leftPadding - rightPadding
		Column {
			width: parent.usableWidth

			spacing: Constants.component_spacing

			TitledSeparator {
				width: parent.width
				contentMarginRight: 0
				contentMarginLeft: 0
				contentMarginBottom: 0
				//: LABEL ANDROID IOS
				title: qsTr("Paired devices")
			}

			GText {
				width: parent.width
				visible: !knownDeviceList.visible

				Accessible.role: Accessible.StaticText
				Accessible.name: text

				//: LABEL ANDROID IOS
				text: qsTr("No device is paired.")
				textStyle: Style.text.normal_secondary
			}

			ListView {
				id: knownDeviceList
				width: parent.width
				height: childrenRect.height
				spacing: Constants.component_spacing
				model: RemoteServiceModel.knownDevices
				delegate: DevicesListDelegate {
					width: knownDeviceList.width

					title: remoteDeviceName
					//: LABEL ANDROID IOS
					description: qsTr("Click to remove device")
					linkQuality: linkQualityInPercent
					linkInactive: !isNetworkVisible

					onClicked: {
						deleteDevicePopup.deviceId = deviceId
						deleteDevicePopup.deviceName = remoteDeviceName
						deleteDevicePopup.open()
					}
				}
				visible: count > 0
				interactive: false
			}
		}

		ConfirmationPopup {
			id: deleteDevicePopup

			property var deviceId
			property string deviceName

			//: INFO ANDROID IOS
			title: qsTr("Remove pairing")
			//: INFO ANDROID IOS
			text: qsTr("Do you want to remove the pairing of the device \"%1\"?").arg(deviceName)
			//: INFO ANDROID IOS
			okButtonText: qsTr("Remove")
			onConfirmed: RemoteServiceModel.forgetDevice(deviceId)
		}

		Column {
			width: parent.usableWidth

			spacing: Constants.component_spacing

			TitledSeparator {
				width: parent.width
				contentMarginRight: 0
				contentMarginLeft: 0
				contentMarginBottom: 0
				//: LABEL ANDROID IOS
				title: qsTr("Available devices")
			}

			GText {
				width: parent.width
				visible: !searchDeviceList.visible

				//: INFO ANDROID IOS No SaC was found on the network, both devices need to be connected to the same WiFi network.
				text: qsTr("No unpaired smartphone as card reader (SaC) available. Please make sure that the smartphone as card reader (SaC) functionality in AusweisApp2 on your other device is activated and that both devices are connected to the same WiFi.")
				textStyle: Style.text.normal_secondary
			}

			GText {
				visible: RemoteServiceModel.requiresLocalNetworkPermission && !RemoteServiceModel.remoteReaderVisible
				width: parent.width
				topPadding: Constants.component_spacing

				text: "%1<br><a href=\"#\"><center>%2</center></a>"
					//: INFO IOS Let user know to check the application settings for local network permission
					.arg(qsTr("To be able to use your smartphone as card reader (SaC), please make sure that access to the local network is allowed for %1.").arg(Qt.application.name))
					//: INFO IOS Link to application settings
					.arg(qsTr("Go to application settings"))
				textStyle: Style.text.normal_secondary
				textFormat: Text.RichText

				onLinkActivated: ApplicationModel.showSettings(ApplicationModel.SETTING_APP)
			}

			ListView {
				id: searchDeviceList
				width: parent.width
				height: childrenRect.height
				model: RemoteServiceModel.availableRemoteDevices
				spacing: Constants.component_spacing
				delegate: DevicesListDelegate {
					width: searchDeviceList.width

					title: remoteDeviceName + (isSupported ? "" : (" (" + remoteDeviceStatus + ")"))
					//: LABEL ANDROID IOS
					description: qsTr("Click to pair")
					linkQuality: linkQualityInPercent

					onClicked: {
						if (isSupported && RemoteServiceModel.rememberServer(deviceId)) {
							informationPairingPopup.open()
						}
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
		title: qsTr("Pairing mode")
		//: INFO ANDROID IOS Information dialog that requests the user to start the pairing mode on the smartphone.
		text: qsTr("Start the pairing mode on your smartphone if you haven't done it already.")
		onConfirmed: firePush(enterPinView)
	}

	Component {
		id: enterPinView

		EnterPasswordView {
			state: "REMOTE_PIN"
			navigationAction: NavigationAction { state: "cancel"; onClicked: { firePop() } }
			//: LABEL ANDROID IOS
			title: qsTr("Pairing code")
			onPasswordEntered: firePop()
		}
	}
}
