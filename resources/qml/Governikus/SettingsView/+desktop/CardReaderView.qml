/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.ReaderModel
import Governikus.Type.ReaderScanEnabler
import Governikus.Type.ReaderPlugIn
import Governikus.View

Column {
	id: root

	spacing: Constants.component_spacing

	ReaderScanEnabler {
		id: readerScanEnabler

		plugInType: ReaderPlugIn.PCSC
	}
	GPane {
		//: LABEL DESKTOP
		title: qsTr("Connected USB card readers")
		width: parent.width

		Column {
			Layout.fillWidth: true
			spacing: Constants.component_spacing
			visible: readerRepeater.count > 0

			Repeater {
				id: readerRepeater

				model: ReaderModel.sortedModel

				delegate: CardReaderDelegate {
					width: parent.width
				}
			}
		}
		RowLayout {
			spacing: Constants.component_spacing
			visible: !readerScanEnabler.scanRunning
			width: parent.width

			TintableIcon {
				source: "qrc:///images/status_warning.svg"
				sourceSize.height: Style.dimens.large_icon_size
				tintColor: Style.color.text_warning
			}
			GText {
				text: qsTr("The connection to your system's smartcard service could not be established. You can try to resolve this issue and restart the scan.")
			}
			GButton {
				text: qsTr("Restart smartcard scan")

				onClicked: readerScanEnabler.restartScan()
			}
		}
		GText {
			id: placeHolderText

			activeFocusOnTab: true
			text: qsTr("No connected card reader found.")
			visible: readerRepeater.count === 0
			width: parent.width

			FocusFrame {
			}
		}
		GSeparator {
			visible: readerRepeater.count === 0
			width: parent.width
		}
		RowLayout {
			id: hintAndDateText

			Layout.topMargin: Constants.component_spacing
			spacing: Constants.component_spacing
			width: parent.width

			TintableIcon {
				source: "qrc:/images/info.svg"
				sourceSize.height: Style.dimens.large_icon_size
				tintColor: Style.color.control
			}
			GText {
				id: hintText

				Layout.alignment: Qt.AlignVCenter
				activeFocusOnTab: true
				color: Style.color.control
				text: qsTr("After connecting a new card reader it may take a few seconds to recognize the driver. It may be necessary to restart your system after installing the driver. Only connected card readers are shown here. %1").arg(ReaderModel.lastUpdatedInformation)
				verticalAlignment: Text.AlignBottom

				FocusFrame {
				}
			}
		}
	}
}
