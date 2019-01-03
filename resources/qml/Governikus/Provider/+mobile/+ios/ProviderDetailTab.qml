import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0

TabButton {
	id: button

	contentItem: Text {
		text: button.text
		font.pixelSize: Constants.small_font_size
		elide: Text.ElideRight
		color: button.checked ? Constants.white : Constants.blue
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}

	background: Rectangle {
		color: Constants.background_color
		clip: true

		Rectangle {
			height: parent.height
			width: parent.width + radius
			anchors.left: parent.left
			anchors.leftMargin: button.TabBar.index === 0 ? 0 : -radius

			radius: Utils.dp(3)
			color: button.checked ? Constants.blue : button.pressed ? Constants.blue_light : Constants.white
			border.color: Constants.blue
			border.width: Utils.dp(1)
		}
	}
}
