/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.NumberModel  1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.View 1.0

Item {
	id: root

	readonly property string helpTopic: "settingsRemoteReader"

	signal pairDevice(string pDeviceId)
	signal unpairDevice(string pDeviceId)
	signal moreInformation()

	ReaderScanEnabler {
		plugInType: ReaderPlugIn.REMOTE
	}

	Column {
		anchors.fill: parent
		spacing: Constants.component_spacing


		GText {
			visible: knownDevices.count > 0

			width: parent.width

			activeFocusOnTab: true

			textStyle: Style.text.header_accent
			text: qsTr("Paired remote devices") + SettingsModel.translationTrigger

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
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

			activeFocusOnTab: true

			textStyle: Style.text.header_accent
			text: qsTr("Available remote devices") + SettingsModel.translationTrigger

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GListView {
			id: availableDevices

			width: parent.width
			height: contentHeight

			model: RemoteServiceModel.availableRemoteDevices
			delegate: RemoteReaderDelegate {
				width: parent.width
				height: implicitHeight + Constants.pane_padding
				onPairDevice: root.pairDevice(pDeviceId)
			}
		}

		GText {
			visible: availableDevices.count === 0

			width: parent.width

			activeFocusOnTab: true

			textStyle: Style.text.normal
			text: RemoteServiceModel.availableRemoteDevices.emptyListDescriptionString

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}

		GSeparator {
			width: parent.width
		}

		RowLayout {
			id: hint

			width: parent.width

			spacing: Constants.text_spacing

			TintableIcon {
				source: "qrc:/images/info_filled.svg"
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
				text: qsTr("Only devices that are already paired or are connected to the same WiFi network and have the remote service enabled are shown here.") + SettingsModel.translationTrigger

				FocusFrame {
					borderColor: Style.color.focus_indicator
				}
			}
		}

		GButton {
			//: LABEL DESKTOP_QML
			text: qsTr("More information") + SettingsModel.translationTrigger
			onClicked: moreInformation()
		}
	}
}
