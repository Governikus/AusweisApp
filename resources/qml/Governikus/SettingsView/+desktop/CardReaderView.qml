/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.ReaderDriverModel 1.0
import Governikus.Type.ReaderScanEnabler 1.0
import Governikus.Type.ReaderPlugIn 1.0
import Governikus.View 1.0


Column {
	id: root

	readonly property string helpTopic: "settingsPcscReader"

	spacing: Constants.component_spacing

	ReaderScanEnabler {
		plugInType: ReaderPlugIn.PCSC
	}

	GText {
		width: parent.width

		activeFocusOnTab: true

		textStyle: Style.text.header_accent
		text: qsTr("Connected USB card reader")

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
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
		textStyle: Style.text.normal

		FocusFrame {
			borderColor: Style.color.focus_indicator
		}
	}

	GSeparator {
		width: parent.width
	}

	RowLayout {
		id: hintAndDateText

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

			verticalAlignment: Text.AlignBottom
			textStyle: Style.text.hint
			text: qsTr("After connecting a new card reader it may take a few seconds to recognize the driver. It may be necessary to restart your system after installing the driver. Only supported and connected card reader are shown here. %1").arg(ReaderDriverModel.lastUpdatedInformation)

			FocusFrame {
				borderColor: Style.color.focus_indicator
			}
		}
	}
}
