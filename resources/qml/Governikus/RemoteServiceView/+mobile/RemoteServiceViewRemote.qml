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
			visible: availablePairedDeviceList.count > 0
			width: parent.usableWidth

			TitledSeparator {
				contentMarginBottom: 0
				contentMarginLeft: 0
				contentMarginRight: 0
				//: LABEL ANDROID IOS
				title: qsTr("Paired devices")
				width: parent.width
			}
			ListView {
				id: availablePairedDeviceList
				height: childrenRect.height
				interactive: false
				model: RemoteServiceModel.availablePairedDevices
				spacing: Constants.component_spacing
				width: parent.width

				delegate: DevicesListDelegate {
					//: LABEL ANDROID IOS
					description: qsTr("Available")
					linkInactive: !isNetworkVisible
					linkQuality: linkQualityInPercent
					title: remoteDeviceName
					width: availablePairedDeviceList.width

					onClicked: {
						deleteDevicePopup.deviceId = deviceId;
						deleteDevicePopup.deviceName = remoteDeviceName;
						deleteDevicePopup.open();
					}
				}
			}
		}
		Column {
			spacing: Constants.component_spacing
			visible: unavailablePairedDeviceList.count > 0
			width: parent.usableWidth

			TitledSeparator {
				contentMarginBottom: 0
				contentMarginLeft: 0
				contentMarginRight: 0
				//: LABEL ANDROID IOS
				title: qsTr("Last connected")
				width: parent.width
			}
			ListView {
				id: unavailablePairedDeviceList
				height: childrenRect.height
				interactive: false
				model: RemoteServiceModel.unavailablePairedDevices
				spacing: Constants.component_spacing
				width: parent.width

				delegate: DevicesListDelegate {
					//: LABEL ANDROID IOS
					description: qsTr("Click to remove device")
					linkInactive: !isNetworkVisible
					linkQuality: linkQualityInPercent
					title: remoteDeviceName
					width: unavailablePairedDeviceList.width

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
				title: qsTr("Add pairing")
				width: parent.width
			}
			GListView {
				id: searchDeviceList
				height: childrenRect.height
				model: RemoteServiceModel.availableDevicesInPairingMode
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
							d.oldLockedAndHiddenStatus = getLockedAndHidden();
							setLockedAndHidden();
							push(enterPinView);
						}
					}
				}
			}
			GText {
				//: INFO ANDROID IOS Wifi is not enabled and no new devices can be paired.
				text: qsTr("Please connect your WiFi to use another smartphone as card reader (SaC).")
				textStyle: Style.text.normal_warning
				visible: !ApplicationModel.wifiEnabled
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
			PairingProcessInfo {
				visible: !searchDeviceList.visible && ApplicationModel.wifiEnabled
				width: parent.width
			}
			RemoteServiceWifiInfo {
				visible: !searchDeviceList.visible && ApplicationModel.wifiEnabled
				width: parent.width
			}
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
