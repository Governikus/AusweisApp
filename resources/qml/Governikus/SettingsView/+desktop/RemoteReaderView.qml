/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.NumberModel  1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.View 1.0

Item {
	id: root

	readonly property string helpTopic: "settingsRemoteReader"

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)
	signal moreInformation()

	implicitWidth: column.implicitWidth
	implicitHeight: column.implicitHeight

	ReaderScanEnabler {
		plugInType: ReaderPlugIn.REMOTE_IFD
	}

	Column {
		id: column

		anchors.fill: parent
		spacing: Constants.component_spacing


		GText {
			visible: knownDevices.count > 0

			width: parent.width

			activeFocusOnTab: true

			textStyle: Style.text.header_accent
			text: qsTr("Paired remote devices")

			FocusFrame {}
		}

		Column {
			width: parent.width

			Repeater {
				id: knownDevices

				model: RemoteServiceModel.knownDevices
				delegate: RemoteReaderDelegate {
					width: parent.width
					height: implicitHeight + Constants.pane_padding
					onUnpairDevice: pDeviceId => root.unpairDevice(pDeviceId)
				}
			}
		}

		GSeparator {
			visible: knownDevices.count > 0

			width: parent.width
		}

		GText {
			width: parent.width

			activeFocusOnTab: true

			textStyle: Style.text.header_accent
			text: qsTr("Available remote devices")

			FocusFrame {}
		}

		GListView {
			id: availableDevices

			width: parent.width
			height: contentHeight

			model: RemoteServiceModel.availableRemoteDevices
			delegate: RemoteReaderDelegate {
				width: availableDevices.width
				height: implicitHeight + Constants.pane_padding
				onPairDevice: pDeviceId => root.pairDevice(pDeviceId)
			}
		}

		GText {
			visible: availableDevices.count === 0

			width: parent.width

			activeFocusOnTab: true

			textStyle: Style.text.normal
			text: RemoteServiceModel.availableRemoteDevices.emptyListDescriptionString

			FocusFrame {}
		}

		GSeparator {
			width: parent.width
		}

		RowLayout {
			id: hint

			width: parent.width

			spacing: Constants.text_spacing

			TintableIcon {
				source: "qrc:/images/info.svg"
				sourceSize.height: Style.dimens.icon_size
				tintColor: Style.color.accent
			}

			GText {
				id: hintText

				Layout.fillWidth: true
				Layout.alignment: Qt.AlignVCenter

				activeFocusOnTab: true

				wrapMode: Text.WordWrap
				verticalAlignment: Text.AlignBottom
				textStyle: Style.text.hint
				text: qsTr("Only devices that are already paired or are connected to the same WiFi network and have the remote service enabled are shown here.")

				FocusFrame {}
			}
		}

		GButton {
			//: LABEL DESKTOP
			text: qsTr("More information")
			onClicked: moreInformation()
		}
	}
}
