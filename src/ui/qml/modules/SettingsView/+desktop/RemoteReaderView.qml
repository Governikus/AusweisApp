/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

Item {
	id: root

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)

	implicitHeight: column.implicitHeight
	implicitWidth: column.implicitWidth

	ReaderScanEnabler {
		pluginType: ReaderManagerPluginType.REMOTE_IFD
	}
	Column {
		id: column

		anchors.fill: parent
		spacing: Constants.component_spacing

		GPane {
			spacing: Constants.component_spacing
			//: LABEL DESKTOP
			title: qsTr("Paired devices")
			visible: availablePairedDevices.count > 0
			width: parent.width

			Repeater {
				id: availablePairedDevices

				model: RemoteServiceModel.availablePairedDevices

				delegate: RemoteReaderDelegate {
					Layout.fillWidth: true

					onUnpairDevice: pDeviceId => root.unpairDevice(pDeviceId)
				}
			}
		}
		GPane {
			spacing: Constants.component_spacing

			//: LABEL DESKTOP
			title: qsTr("Last connected")
			visible: unavailablePairedDevices.count > 0
			width: parent.width

			Repeater {
				id: unavailablePairedDevices

				model: RemoteServiceModel.unavailablePairedDevices

				delegate: RemoteReaderDelegate {
					Layout.fillWidth: true

					onUnpairDevice: pDeviceId => root.unpairDevice(pDeviceId)
				}
			}
		}
		GPane {
			spacing: Constants.component_spacing

			//: LABEL DESKTOP
			title: qsTr("Add pairing")
			width: parent.width

			GListView {
				id: availableDevices

				Layout.fillWidth: true
				implicitHeight: contentHeight
				model: RemoteServiceModel.availableDevicesInPairingMode

				delegate: RemoteReaderDelegate {
					height: implicitHeight + Constants.pane_padding
					width: availableDevices.width

					onPairDevice: pDeviceId => root.pairDevice(pDeviceId)
				}
			}
			Column {
				Layout.fillWidth: true
				spacing: Constants.text_spacing
				visible: availableDevices.count === 0

				Repeater {
					model: [
						//: LABEL ANDROID IOS Assistance text for pairing new devices. Step 1 of 4. %1 is a placeholder-tag for the app name.
						qsTr("Ensure that the %1 on your Smartphone as card reader has at least version %2.").arg(Qt.application.name).arg("2.1.0"),
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
							Layout.maximumWidth: Number.POSITIVE_INFINITY
							activeFocusOnTab: true
							text: modelData

							FocusFrame {
							}
						}
					}
				}
			}
			GSeparator {
				Layout.fillWidth: true
				visible: availableDevices.count === 0
			}
			RowLayout {
				Layout.fillWidth: true
				spacing: Constants.component_spacing
				visible: availableDevices.count === 0

				TintableIcon {
					source: "qrc:/images/info.svg"
					sourceSize.height: Style.dimens.large_icon_size
					tintColor: hintText.color
				}
				GText {
					id: hintText

					Layout.alignment: Qt.AlignVCenter
					activeFocusOnTab: true
					color: Style.color.textSubline.basic
					text: qsTr("Both devices have to be on the same network (e.g. WiFi).")
					verticalAlignment: Text.AlignBottom
					wrapMode: Text.WordWrap

					FocusFrame {
					}
				}
			}
		}
	}
}
