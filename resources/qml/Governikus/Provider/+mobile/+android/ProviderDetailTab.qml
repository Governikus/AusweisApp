import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

TabButton {
	id: button
	padding: Utils.dp(10)

	contentItem: Text {
		text: button.text
		font.pixelSize: Constants.small_font_size
		elide: Text.ElideRight
		color: button.checked || button.pressed ? Constants.blue : Constants.black
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}

	background: Rectangle {
		color: Constants.background_color
		clip: true

		Rectangle {
			height: parent.height + radius
			width: parent.width

			radius: Utils.dp(3)
			color: button.checked ? Constants.white : Constants.grey
			border.color: Constants.grey
			border.width: Utils.dp(1)
		}
	}
}
