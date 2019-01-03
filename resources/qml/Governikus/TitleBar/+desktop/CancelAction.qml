import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: button
	visible: ApplicationModel.currentWorkflow !== ""

	Accessible.role: Accessible.Button
	Accessible.name: qsTr("Cancel")

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
			readonly property Text sizeBase: Text {
				font.weight: Font.Bold
				font.pixelSize: Constants.titlebar_font_size
				text: qsTr("Cancel") + settingsModel.translationTrigger
			}

			FocusFrame {
				scope: button
			}

			Text {
				anchors.centerIn: parent
				color: Constants.white
				font.weight: Font.Bold
				font.pixelSize: Constants.titlebar_font_size * (button.down ? 0.9 : 1)
				text: text.sizeBase.text
			}
		}
	}
}
