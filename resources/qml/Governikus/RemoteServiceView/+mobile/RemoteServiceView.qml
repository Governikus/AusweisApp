/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.RemoteServiceModel 1.0


SectionPage {
	id: baseItem

	readonly property string currentPin: RemoteServiceModel.psk.toString()
	readonly property bool isPairing: RemoteServiceModel.running && currentPin !== ""

	navigationAction: NavigationAction {
		state: RemoteServiceModel.running ? "cancel" : ""
		onClicked: {
			RemoteServiceModel.setPairing(false)
			RemoteServiceModel.running = false
		}
	}

	//: LABEL ANDROID IOS
	title: qsTr("Remote service") + SettingsModel.translationTrigger

	Connections {
		target: RemoteServiceModel
		//: ERROR ANDROID IOS An error occurred while pairing the device.
		onFirePairingFailed: ApplicationModel.showFeedback(qsTr("Pairing failed. Please start a new pairing process on your other device and enter the shown pairing code."))
	}

	RemoteServiceController {
		id: controller
	}

	content: Item {
		implicitHeight: baseItem.height
		implicitWidth: baseItem.width

		ColumnLayout {
			height: parent.height - 2 * Constants.pane_padding
			width: Math.min(parent.width - 2 * Constants.pane_padding, Style.dimens.max_text_width)
			anchors.centerIn: parent

			spacing: Constants.pane_spacing

			Image {
				Layout.fillWidth: true
				Layout.maximumHeight: width * (sourceSize.height / sourceSize.width)

				source: "qrc:///images/phone_to_pc.svg"
				fillMode: Image.PreserveAspectFit
			}

			RowLayout {
				Layout.fillWidth: true

				spacing: Constants.text_spacing

				TintableIcon {
					source: "qrc:/images/info_filled.svg"
					sourceSize.width: Style.dimens.small_icon_size
					tintColor: Style.text.normal_secondary.textColor
				}

				GText {
					Layout.fillWidth: true

					//: INFO ANDROID IOS The remote service is active. Hint that both devices need to be connected to the same network.
					text: qsTr("Both of your devices have to be connected to the same WiFi.") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_secondary
				}
			}

			GButton {
				id: startButton

				property bool serviceIsStarting: false
				readonly property bool running: RemoteServiceModel.running
				readonly property bool canEnableNfc: RemoteServiceModel.canEnableNfc

				Layout.preferredWidth: Math.max(pairingButton.implicitWidth, startButton.implicitWidth)
				Layout.alignment: Qt.AlignHCenter

				buttonColor: running ? Constants.red : Constants.darkgreen
				enabled: (canEnableNfc || RemoteServiceModel.runnable || running) && !serviceIsStarting
				text: {
					SettingsModel.translationTrigger; // Bind this evaluation to the trigger.

					if (canEnableNfc) {
						//: LABEL ANDROID IOS
						return qsTr("Enable NFC");
					} else if (running) {
						//: LABEL ANDROID IOS
						return qsTr("Stop remote service");
					} else {
						//: LABEL ANDROID IOS
						return qsTr("Start remote service");
					}
				}
				onClicked: {
					if (canEnableNfc) {
						ApplicationModel.showSettings(ApplicationModel.SETTING_NFC)
					} else {
						if (!running) {
							serviceIsStarting = true
						} else {
							RemoteServiceModel.setPairing(false)
						}
						RemoteServiceModel.running = !running
					}
				}
				onRunningChanged: {
					serviceIsStarting = false
					navBar.lockedAndHidden = running
				}
			}

			GButton {
				id: pairingButton

				visible: opacity > 0
				Layout.preferredWidth: Math.max(pairingButton.implicitWidth, startButton.implicitWidth)
				Layout.alignment: Qt.AlignHCenter

				opacity: 0
				text: (isPairing ?
					   //: LABEL ANDROID IOS
					   qsTr("Stop pairing") :
					   //: LABEL ANDROID IOS
					   qsTr("Start pairing")
					  ) + SettingsModel.translationTrigger
				onClicked: RemoteServiceModel.setPairing(!isPairing)
			}

			GText {
				id: pairingCode

				visible: isPairing && opacity > 0
				Layout.fillWidth: true

				//: LABEL ANDROID IOS
				Accessible.name: qsTr("Pairing code: %1").arg(currentPin.split("").join(" ")) + SettingsModel.translationTrigger;

				//: LABEL ANDROID IOS
				text:  qsTr("Pairing code: <b>%1</b>").arg(currentPin) + SettingsModel.translationTrigger
				textStyle: Style.text.header_accent
				horizontalAlignment: Text.AlignHCenter
			}

			Item {/*spacer*/ Layout.fillHeight: true }

			GText {
				visible: !RemoteServiceModel.runnable
				Layout.fillWidth: true

				text: RemoteServiceModel.errorMessage
				textStyle: Style.text.normal_warning
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				id: infoMessage

				visible: RemoteServiceModel.runnable && opacity > 0
				Layout.fillWidth: true

				text: isPairing ?
				//: INFO ANDROID IOS
				 qsTr("Enter this code on your other device to use your smartphone as a card reader (SaC)")
				 //: INFO ANDROID IOS
				: RemoteServiceModel.running ? qsTr("Waiting for connection from a paired device...")
				 //: INFO ANDROID IOS
				: qsTr("Start the remote access, in order to make this smartphone visible and to use it as a card reader (SaC)") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_secondary
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				id: headText

				visible: opacity > 0
				Layout.fillWidth: true

				opacity: 0
				//: LABEL ANDROID IOS
				text: qsTr("Card access in progress") + SettingsModel.translationTrigger;
				textStyle: Style.text.header_accent
				horizontalAlignment: Text.AlignHCenter
			}

			GText {
				id: subText

				visible: opacity > 0
				Layout.fillWidth: true

				opacity: 0
				text: RemoteServiceModel.connectionInfo
				textStyle: Style.text.normal_secondary
				horizontalAlignment: Text.AlignHCenter
			}

			states: [
				State { name: "UNCONNECTED"; when: RemoteServiceModel.running && !RemoteServiceModel.connectedToPairedDevice
					PropertyChanges { target: pairingButton; opacity: 1 }
					PropertyChanges { target: pairingCode; opacity: 1 }
					PropertyChanges { target: infoMessage; opacity: 1 }
					PropertyChanges { target: headText; opacity: 0 }
					PropertyChanges { target: subText; opacity: 0 }
					PropertyChanges { target: ApplicationModel; nfcRunning: false; restoreEntryValues: false }
				},
				State { name: "CONNECTED"; when: RemoteServiceModel.running && RemoteServiceModel.connectedToPairedDevice
					PropertyChanges { target: pairingButton; opacity: 0 }
					PropertyChanges { target: pairingCode; opacity: 0 }
					PropertyChanges { target: infoMessage; opacity: 0 }
					PropertyChanges { target: headText; opacity: 1 }
					PropertyChanges { target: subText; opacity: 1 }
					PropertyChanges { target: ApplicationModel; nfcRunning: true; restoreEntryValues: false }
				}
			]
			transitions: [
				Transition {
					NumberAnimation {
						property: "opacity"
						duration: Constants.animation_duration
						easing.type: Easing.InOutQuad
					}
				}
			]
		}
	}
}
