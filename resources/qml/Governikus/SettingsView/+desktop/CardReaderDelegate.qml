/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.View 1.0


Item {
	property int iconHeight: ApplicationModel.scaleFactor * 175

	implicitHeight: rowLayout.implicitHeight
	activeFocusOnTab: true

	FocusFrame {
		dynamic: false
		border.color: Constants.black
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

				textStyle: Style.text.header_inverse
				text: readerName
			}

			GText {
				Layout.fillWidth: true

				textStyle: Style.text.normal_inverse
				text: readerStatus
			}

			GText {
				visible: !readerInstalledAndSupported

				Layout.fillWidth: true

				textStyle: Style.text.normal_inverse
				text: readerHTMLDescription
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
