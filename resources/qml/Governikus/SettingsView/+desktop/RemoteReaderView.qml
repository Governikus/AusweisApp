/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.NumberModel  1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.View 1.0

Item {
	id: root

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)

	Component.onCompleted: {
		RemoteServiceModel.detectRemoteDevices = visible
	}

	Component.onDestruction: {
		RemoteServiceModel.detectRemoteDevices = false
	}

	onVisibleChanged: RemoteServiceModel.detectRemoteDevices = visible

	Column {
		anchors.fill: parent
		spacing: Constants.component_spacing


		GText {
			visible: knownDevices.count > 0

			width: parent.width

			textStyle: Style.text.header_accent
			text: qsTr("Paired remote devices") + SettingsModel.translationTrigger
		}

		Column {
			width: parent.width

			Repeater {
				id: knownDevices

				model: RemoteServiceModel.knownDevices
				delegate: RemoteReaderDelegate {
					width: parent.width
					height: implicitHeight + Constants.pane_padding
					onUnpairDevice: root.unpairDevice(pDeviceId)
				}
			}
		}

		GSeparator {
			visible: knownDevices.count > 0

			width: parent.width
		}

		GText {
			width: parent.width
			textStyle: Style.text.header_accent
			text: qsTr("Available remote devices") + SettingsModel.translationTrigger
		}

		Column {
			width: parent.width

			Repeater {
				id: availableDevices

				model: RemoteServiceModel.availableRemoteDevices
				delegate: RemoteReaderDelegate {
					width: parent.width
					height: implicitHeight + Constants.pane_padding
					onPairDevice: root.pairDevice(pDeviceId)
				}
			}
		}

		GText {
			visible: availableDevices.count === 0

			width: parent.width

			activeFocusOnTab: true
			textStyle: Style.text.normal_inverse
			text: qsTr("No devices with enabled remote service were found on the current WiFi network") + SettingsModel.translationTrigger

			FocusFrame {
				dynamic: false
				border.color: Constants.black
			}
		}

		RowLayout {
			id: hint

			width: parent.width

			spacing: Constants.component_spacing

			Image {
				Layout.preferredHeight: hintText.height * 1.5

				fillMode: Image.PreserveAspectFit
				source: "qrc:/images/desktop/info_version.svg"
			}

			GText {
				id: hintText

				Layout.fillWidth: true
				Layout.alignment: Qt.AlignVCenter

				activeFocusOnTab: true

				wrapMode: Text.WordWrap
				verticalAlignment: Text.AlignBottom
				textStyle: Style.text.hint_inverse
				text: qsTr("Only devices that are already paired or are connected to the same WiFi network and have the remote service enabled are shown here.") + SettingsModel.translationTrigger

				FocusFrame {
					dynamic: false
					border.color: Constants.black
				}
			}
		}
	}
}
