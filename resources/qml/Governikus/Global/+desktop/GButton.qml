import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


Button {
	id: control
	padding: Constants.pane_padding / 3

	contentItem: Row {
		spacing: Constants.groupbox_spacing

		Image {
			source: control.icon.source
			visible: source.toString().length > 0
			sourceSize.height: textContainer.height
		}

		Item {
			id: textContainer
			height: originalSize.height
			width: originalSize.width

			Text {
				id: originalSize
				visible: false
				text: control.text
				font.bold: true
				font.pixelSize: Constants.normal_font_size
			}

			Text {
				text: control.text
				color: Constants.white
				font.bold: true
				font.pixelSize: Constants.normal_font_size - ApplicationModel.scaleFactor * (control.down ? 1 : 0)
				anchors.centerIn: parent

				FocusFrame {
					scope: control
				}
			}
		}
	}

	background: Rectangle {
		color: Constants.blue
		radius: ApplicationModel.scaleFactor * 4
	}
}
