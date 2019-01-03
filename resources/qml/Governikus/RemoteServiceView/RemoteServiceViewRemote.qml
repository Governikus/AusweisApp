import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.EnterPinView 1.0
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
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

			Text {
				text: qsTr("Paired devices") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.blue
			}

			Text {
				color: Constants.secondary_text
				text: qsTr("No device is paired.") + settingsModel.translationTrigger
				width: parent.width
				visible: !knownDeviceList.visible
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			ListView {
				id: knownDeviceList
				width: parent.width
				height: childrenRect.height
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

			Text {
				text: qsTr("Available devices") + settingsModel.translationTrigger
				font.pixelSize: Constants.normal_font_size
				font.bold: true
				color: Constants.blue
			}

			Text {
				color: Constants.secondary_text
				text: qsTr("No new remote reader was found on your network. Make sure that the remote reader functionality in AusweisApp2 on your other device is activated and that your devices are connected to the same network.") + settingsModel.translationTrigger
				width: parent.width
				visible: !searchDeviceList.visible
				font.pixelSize: Constants.normal_font_size
				wrapMode: Text.WordWrap
			}

			ListView {
				id: searchDeviceList
				width: parent.width
				height: childrenRect.height
				model: RemoteServiceModel.availableRemoteDevices
				delegate: AvailableDevicesListDelegate {
					width: searchDeviceList.width
					onRequestPairing: {
						enterPinView.remoteDeviceId = pDeviceId
						informationPairingPopup.open()
					}
				}
				visible: count > 0
				interactive: false
			}
		}
	}

	Popup {
		id: informationPairingPopup
		x: (parentSectionPage.width - width) / 2
		y: (parentSectionPage.height - height) / 2
		width: Utils.dp(250)
		height: contentColumn.height + 2 * Constants.pane_padding
		modal: true
		focus: true
		padding: Constants.pane_padding
		closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnEscape

		Column {
			id: contentColumn
			width: parent.width
			spacing: Constants.pane_spacing

			Text {
				id: info
				color: Constants.secondary_text
				width: parent.width
				wrapMode: Text.WordWrap
				font.pixelSize: Constants.normal_font_size
				text: qsTr("Please start pairing mode first.") + settingsModel.translationTrigger
			}

			GButton {
				text: qsTr("OK") + settingsModel.translationTrigger
				width: parent.width

				onClicked: {
					informationPairingPopup.close()
					pinEntryInProgress = true
					firePush(enterPinView)
				}
			}
		}
	}

	EnterPinView {
		id: enterPinView
		state: "REMOTE_PIN"
		leftTitleBarAction: TitleBarAction { state: "cancel"; onClicked: { firePop(); pinEntryInProgress = false } }
		headerTitleBarAction: TitleBarAction { text: qsTr("Pairing code") + settingsModel.translationTrigger }
		visible: false

		onPinEntered: {
			firePop()
			pinEntryInProgress = false
		}
	}
}
