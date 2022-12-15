/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.View 1.0

Item {
	id: root

	readonly property string helpTopic: "settingsRemoteReader"

	signal moreInformation
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

		GText {
			activeFocusOnTab: true
			text: qsTr("Paired remote devices")
			textStyle: Style.text.header_accent
			visible: knownDevices.count > 0
			width: parent.width

			FocusFrame {
			}
		}
		Column {
			width: parent.width

			Repeater {
				id: knownDevices
				model: RemoteServiceModel.knownDevices

				delegate: RemoteReaderDelegate {
					height: implicitHeight + Constants.pane_padding
					width: parent.width

					onUnpairDevice: pDeviceId => root.unpairDevice(pDeviceId)
				}
			}
		}
		GSeparator {
			visible: knownDevices.count > 0
			width: parent.width
		}
		GText {
			activeFocusOnTab: true
			text: qsTr("Available remote devices")
			textStyle: Style.text.header_accent
			width: parent.width

			FocusFrame {
			}
		}
		GListView {
			id: availableDevices
			height: contentHeight
			model: RemoteServiceModel.availableRemoteDevices
			width: parent.width

			delegate: RemoteReaderDelegate {
				height: implicitHeight + Constants.pane_padding
				width: availableDevices.width

				onPairDevice: pDeviceId => root.pairDevice(pDeviceId)
			}
		}
		GText {
			activeFocusOnTab: true
			text: RemoteServiceModel.availableRemoteDevices.emptyListDescriptionString
			textStyle: Style.text.normal
			visible: availableDevices.count === 0
			width: parent.width

			FocusFrame {
			}
		}
		GSeparator {
			width: parent.width
		}
		RowLayout {
			id: hint
			spacing: Constants.text_spacing
			width: parent.width

			TintableIcon {
				source: "qrc:/images/info.svg"
				sourceSize.height: Style.dimens.icon_size
				tintColor: Style.color.accent
			}
			GText {
				id: hintText
				Layout.alignment: Qt.AlignVCenter
				Layout.fillWidth: true
				activeFocusOnTab: true
				text: qsTr("Only devices that are already paired or are connected to the same WiFi network and have the remote service enabled are shown here.")
				textStyle: Style.text.hint
				verticalAlignment: Text.AlignBottom
				wrapMode: Text.WordWrap

				FocusFrame {
				}
			}
		}
		GButton {
			//: LABEL DESKTOP
			text: qsTr("More information")

			onClicked: moreInformation()
		}
	}
}
