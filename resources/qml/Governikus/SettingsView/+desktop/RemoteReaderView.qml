/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.View 1.0

Item {
	id: root

	readonly property string helpTopic: "settingsRemoteReader"

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)

	implicitHeight: column.implicitHeight
	implicitWidth: column.implicitWidth

	ReaderScanEnabler {
		plugInType: ReaderPlugIn.REMOTE_IFD
	}
	Column {
		id: column
		anchors.fill: parent
		spacing: Constants.component_spacing

		Column {
			spacing: Constants.component_spacing
			visible: availablePairedDevices.count > 0
			width: parent.width

			GText {
				activeFocusOnTab: true
				text: qsTr("Paired devices")
				textStyle: Style.text.header_accent
				width: parent.width

				FocusFrame {
				}
			}
			Repeater {
				id: availablePairedDevices
				model: RemoteServiceModel.availablePairedDevices

				delegate: RemoteReaderDelegate {
					width: parent.width

					onUnpairDevice: pDeviceId => root.unpairDevice(pDeviceId)
				}
			}
		}
		Column {
			spacing: Constants.component_spacing
			visible: unavailablePairedDevices.count > 0
			width: parent.width

			GText {
				activeFocusOnTab: true
				text: qsTr("Last connected")
				textStyle: Style.text.header_accent
				width: parent.width

				FocusFrame {
				}
			}
			Repeater {
				id: unavailablePairedDevices
				model: RemoteServiceModel.unavailablePairedDevices

				delegate: RemoteReaderDelegate {
					width: parent.width

					onUnpairDevice: pDeviceId => root.unpairDevice(pDeviceId)
				}
			}
		}
		GSeparator {
			visible: availablePairedDevices.count > 0 || unavailablePairedDevices.count > 0
			width: parent.width
		}
		GText {
			activeFocusOnTab: true
			text: qsTr("Add pairing")
			textStyle: Style.text.header_accent
			width: parent.width

			FocusFrame {
			}
		}
		GListView {
			id: availableDevices
			height: contentHeight
			model: RemoteServiceModel.availableDevicesInPairingMode
			width: parent.width

			delegate: RemoteReaderDelegate {
				height: implicitHeight + Constants.pane_padding
				width: availableDevices.width

				onPairDevice: pDeviceId => root.pairDevice(pDeviceId)
			}
		}
		Column {
			spacing: Constants.text_spacing
			visible: availableDevices.count === 0
			width: parent.width

			Repeater {
				model: [
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 4. %1 is a placeholder-tag for the app name.
					qsTr("Ensure that the %1 on your Smartphone as card reader has at least version %2.").arg(Qt.application.name).arg(Qt.application.version),
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 2 of 4. %1 is a placeholder-tag for the app name.
					qsTr("Open the %1 on your Smartphone as card reader.").arg(Qt.application.name),
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 3 of 4. %1 and %2 are surrounding tags for bold font
					qsTr("On that device go to %1Card reader%2 and then %1Pair device%2 rsp. %1Pair new device%2.").arg("<b>").arg("</b>"),
					//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 4 of 4
					qsTr("Choose the device in the list to pair it.")]

				RowLayout {
					spacing: Constants.component_spacing
					width: parent.width

					GText {
						Accessible.ignored: true
						Layout.alignment: Qt.AlignTop
						text: (index + 1) + "."
					}
					GText {
						Accessible.name: (index + 1) + ". " + ApplicationModel.stripHtmlTags(modelData)
						Layout.alignment: Qt.AlignTop
						Layout.fillWidth: true
						activeFocusOnTab: true
						text: modelData

						FocusFrame {
						}
					}
				}
			}
		}
		RowLayout {
			spacing: Constants.text_spacing
			visible: availableDevices.count === 0
			width: parent.width

			TintableIcon {
				source: "qrc:/images/info.svg"
				sourceSize.height: Style.dimens.icon_size
				tintColor: Style.color.accent
			}
			GText {
				Layout.alignment: Qt.AlignVCenter
				Layout.fillWidth: true
				activeFocusOnTab: true
				text: qsTr("Both devices have to be connected to the same WiFi.")
				textStyle: Style.text.hint
				verticalAlignment: Text.AlignBottom
				wrapMode: Text.WordWrap

				FocusFrame {
				}
			}
		}
	}
}
