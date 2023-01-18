/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ReaderModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.View 1.0

Column {
	id: root

	readonly property string helpTopic: "settingsPcscReader"

	spacing: Constants.component_spacing

	ReaderScanEnabler {
		id: readerScanEnabler
		plugInType: ReaderPlugIn.PCSC
	}
	GText {
		activeFocusOnTab: true
		text: qsTr("Connected USB card readers")
		textStyle: Style.text.header_accent
		width: parent.width

		FocusFrame {
		}
	}
	Column {
		spacing: Constants.component_spacing
		visible: readerRepeater.count > 0
		width: parent.width

		Repeater {
			id: readerRepeater
			model: ReaderModel.sortedModel

			delegate: CardReaderDelegate {
				width: parent.width

				GSeparator {
					anchors.bottom: parent.bottom
					width: parent.width
				}
			}
		}
	}
	RowLayout {
		visible: !readerScanEnabler.scanRunning
		width: parent.width

		TintableIcon {
			source: "qrc:///images/material_alert.svg"
			sourceSize.height: Style.dimens.icon_size
			tintColor: Style.color.warning_text
		}
		GText {
			Layout.fillWidth: true
			text: qsTr("The connection to your system's smartcard service could not be established. You can try to resolve this issue and restart the scan.")
			textStyle: Style.text.hint
		}
		GButton {
			text: qsTr("Restart smartcard scan")

			onClicked: readerScanEnabler.restartScan()
		}
	}
	GText {
		id: placeHolderText
		activeFocusOnTab: true
		text: ReaderModel.emptyListDescriptionString
		textStyle: Style.text.normal
		verticalAlignment: Text.AlignVCenter
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
			text: qsTr("After connecting a new card reader it may take a few seconds to recognize the driver. It may be necessary to restart your system after installing the driver. Only connected card readers are shown here. %1").arg(ReaderModel.lastUpdatedInformation)
			textStyle: Style.text.hint
			verticalAlignment: Text.AlignBottom

			FocusFrame {
			}
		}
	}
}
