/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.EnterPasswordView 1.0
import Governikus.Global 1.0
import Governikus.PasswordInfoView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.PasswordType 1.0
import Governikus.Type.RemoteServiceModel 1.0

Item {
	id: baseItem
	height: mainColumn.height

	QtObject {
		id: d

		property bool oldLockedAndHiddenStatus
	}
	Column {
		id: mainColumn

		readonly property int usableWidth: width - leftPadding - rightPadding

		leftPadding: Constants.pane_padding
		rightPadding: Constants.pane_padding
		spacing: Constants.component_spacing
		width: baseItem.width

		Column {
			spacing: Constants.component_spacing
			width: parent.usableWidth

			TitledSeparator {
				contentMarginBottom: 0
				contentMarginLeft: 0
				contentMarginRight: 0
				//: LABEL ANDROID IOS
				title: qsTr("Paired devices")
				width: parent.width
			}
			GText {
				Accessible.name: text
				Accessible.role: Accessible.StaticText

				//: LABEL ANDROID IOS
				text: qsTr("No device is paired.")
				textStyle: Style.text.normal_secondary
				visible: !knownDeviceList.visible
				width: parent.width
			}
			ListView {
				id: knownDeviceList
				height: childrenRect.height
				interactive: false
				model: RemoteServiceModel.knownDevices
				spacing: Constants.component_spacing
				visible: count > 0
				width: parent.width

				delegate: DevicesListDelegate {
					//: LABEL ANDROID IOS
					description: qsTr("Click to remove device")
					linkInactive: !isNetworkVisible
					linkQuality: linkQualityInPercent
					title: remoteDeviceName
					width: knownDeviceList.width

					onClicked: {
						deleteDevicePopup.deviceId = deviceId;
						deleteDevicePopup.deviceName = remoteDeviceName;
						deleteDevicePopup.open();
					}
				}
			}
		}
		ConfirmationPopup {
			id: deleteDevicePopup

			property var deviceId
			property string deviceName

			//: INFO ANDROID IOS
			okButtonText: qsTr("Remove")
			//: INFO ANDROID IOS
			text: qsTr("Do you want to remove the pairing of the device \"%1\"?").arg(deviceName)

			//: INFO ANDROID IOS
			title: qsTr("Remove pairing")

			onConfirmed: RemoteServiceModel.forgetDevice(deviceId)
		}
		Column {
			spacing: Constants.component_spacing
			width: parent.usableWidth

			TitledSeparator {
				contentMarginBottom: 0
				contentMarginLeft: 0
				contentMarginRight: 0

				//: LABEL ANDROID IOS
				title: qsTr("Available devices")
				width: parent.width
			}
			GText {
				text: ApplicationModel.wifiEnabled ?
				//: INFO ANDROID IOS No SaC was found on the network, both devices need to be connected to the same WiFi network.
				qsTr("No unpaired smartphone as card reader (SaC) available. Please make sure that the smartphone as card reader (SaC) functionality in AusweisApp2 on your other device is activated and that both devices are connected to the same WiFi.") :
				//: INFO ANDROID IOS Wifi is not enabled and no new devices can be paired.
				qsTr("Please connect your WiFi to use another smartphone as card reader (SaC).")
				textStyle: ApplicationModel.wifiEnabled ? Style.text.normal_secondary : Style.text.normal_warning
				visible: !searchDeviceList.visible
				width: parent.width
			}
			GButton {
				anchors.horizontalCenter: parent.horizontalCenter

				//: LABEL ANDROID IOS
				text: qsTr("Enable WiFi")
				visible: !ApplicationModel.wifiEnabled

				onClicked: ApplicationModel.enableWifi()
			}
			LocalNetworkInfo {
				topPadding: Constants.component_spacing
				visible: RemoteServiceModel.requiresLocalNetworkPermission && !RemoteServiceModel.remoteReaderVisible
				width: parent.width
			}
			ListView {
				id: searchDeviceList
				height: childrenRect.height
				interactive: false
				model: RemoteServiceModel.availableRemoteDevices
				spacing: Constants.component_spacing
				visible: ApplicationModel.wifiEnabled && count > 0
				width: parent.width

				delegate: DevicesListDelegate {
					//: LABEL ANDROID IOS
					description: qsTr("Click to pair")
					linkQuality: linkQualityInPercent
					title: remoteDeviceName + (isSupported ? "" : (" (" + remoteDeviceStatus + ")"))
					width: searchDeviceList.width

					onClicked: {
						if (isSupported && RemoteServiceModel.rememberServer(deviceId)) {
							informationPairingPopup.open();
						}
					}
				}
			}
		}
	}
	ConfirmationPopup {
		id: informationPairingPopup
		style: ConfirmationPopup.PopupStyle.OkButton
		//: INFO ANDROID IOS Information dialog that requests the user to start the pairing mode on the smartphone.
		text: qsTr("Start the pairing mode on your smartphone if you haven't done it already.")
		//: INFO ANDROID IOS
		title: qsTr("Pairing mode")

		onConfirmed: {
			d.oldLockedAndHiddenStatus = getLockedAndHidden();
			setLockedAndHidden();
			push(enterPinView);
		}
	}
	PasswordInfoData {
		id: infoData
		contentType: PasswordInfoContent.Type.SMARTPHONE_AS_CARD_READER
	}
	Component {
		id: passwordInfoView
		PasswordInfoView {
			infoContent: infoData

			onClose: pop()
		}
	}
	Component {
		id: enterPinView
		EnterPasswordView {
			function close() {
				setLockedAndHidden(d.oldLockedAndHiddenStatus);
				pop();
			}

			moreInformationText: infoData.linkText
			passwordType: PasswordType.REMOTE_PIN
			//: LABEL ANDROID IOS
			title: qsTr("Pairing code")

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Cancel

				onClicked: close()
			}

			onPasswordEntered: close()
			onRequestPasswordInfo: push(passwordInfoView)
		}
	}
}
