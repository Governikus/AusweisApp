/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


Button {
	id: button
	visible: ApplicationModel.currentWorkflow !== ""

	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Cancel") + SettingsModel.translationTrigger

	background: Row {
		id: row
		spacing: ApplicationModel.scaleFactor * 20

		Rectangle {
			height: text.height
			width: Math.max(ApplicationModel.scaleFactor * 2, 1)
		}

		Image {
			sourceSize.height: text.height
			source: "qrc:///images/cancel.svg"
		}

		Item {
			id: text
			height: sizeBase.height
			width: sizeBase.width
			readonly property GText sizeBase: GText {
				text: qsTr("Cancel") + SettingsModel.translationTrigger
				textStyle: Style.text.header
				font.bold: true
			}

			FocusFrame {
				scope: button
			}

			GText {
				anchors.centerIn: parent

				text: text.sizeBase.text
				textStyle: Style.text.header
				font.bold: true
				font.pixelSize: Style.text.header.textSize * (button.down ? 0.9 : 1)
			}
		}
	}

	MouseArea {
		cursorShape: Qt.PointingHandCursor
		anchors.fill: parent
		onPressed: mouse.accepted = false
	}
}
