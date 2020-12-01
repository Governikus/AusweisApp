/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	property int iconHeight: ApplicationModel.scaleFactor * 175

	implicitHeight: rowLayout.implicitHeight
	activeFocusOnTab: true

	Accessible.role: Accessible.Button
	Accessible.name: readerName + ". " + readerStatus

	FocusFrame {
		borderColor: Style.color.focus_indicator
	}

	RowLayout {
		id: rowLayout

		width: parent.width

		spacing: 0

		Image {
			id: readerIcon

			sourceSize.height: iconHeight

			asynchronous: true
			source: readerImagePath
			fillMode: Image.PreserveAspectFit
		}

		ColumnLayout {
			id: textColumn

			Layout.fillHeight: true
			Layout.alignment: Qt.AlignLeft
			spacing: Constants.text_spacing

			GText {
				Layout.fillWidth: true

				textStyle: Style.text.header
				text: readerName
			}

			GText {
				Layout.fillWidth: true

				textStyle: Style.text.normal
				text: readerStatus
			}

			GText {
				visible: !readerInstalledAndSupported

				Layout.fillWidth: true

				activeFocusOnTab: true
				Accessible.description: qsTr("Press space to open the link in your browser")

				textStyle: Style.text.normal
				text: readerHTMLDescription

				FocusFrame {
					borderColor: Style.color.focus_indicator
				}
			}
		}

		Image {
			id: statusIcon

			sourceSize.height: iconHeight * 0.33

			fillMode: Image.PreserveAspectFit
			asynchronous: true
			source: readerInstalledAndSupported ? "qrc:/images/status_ok.svg" : "qrc:/images/status_error.svg"
		}
	}
}
