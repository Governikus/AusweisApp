/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ReaderDriverModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.View 1.0


Column {
	id: root

	spacing: Constants.component_spacing

	ReaderScanEnabler {
	}

	Column {
		visible: readerRepeater.count > 0

		width: parent.width

		Repeater {
			id: readerRepeater

			model: ReaderDriverModel
			delegate: CardReaderDelegate {
				width: parent.width

				GSeparator {
					visible: index < readerRepeater.count - 1
					width: parent.width
					anchors.bottom: parent.bottom
				}
			}
		}
	}

	GText {
		id: placeHolderText

		visible: readerRepeater.count === 0

		width: parent.width

		activeFocusOnTab: true

		text: ReaderDriverModel.emptyListDescriptionString
		verticalAlignment: Text.AlignVCenter
		textStyle: Style.text.normal_inverse

		Keys.onSpacePressed: ApplicationModel.openOnlineHelp("readerDeviceTab")
		onLinkActivated: Qt.openUrlExternally(link)

		FocusFrame {
			dynamic: false
			border.color: Constants.black
		}
	}

	GSeparator {
		width: parent.width
	}

	RowLayout {
		id: hintAndDateText

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

			verticalAlignment: Text.AlignBottom
			textStyle: Style.text.hint_inverse
			text: qsTr("After connecting a new card reader it may take a few seconds to recognize the driver. It may be necessary to restart your system after installing the driver. Only supported and connected card reader are shown here. %1").arg(ReaderDriverModel.lastUpdatedInformation) + SettingsModel.translationTrigger

			FocusFrame {
				dynamic: false
				border.color: Constants.black
			}
		}
	}
}
