/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0

TabButton {
	id: button

	Accessible.role: Accessible.PageTab
	Accessible.name: text

	contentItem: GText {
		text: button.text
		textStyle: button.checked ? Style.text.hint_inverse : Style.text.hint_accent
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}

	background: Rectangle {
		color: Style.color.background
		clip: true

		Rectangle {
			height: parent.height
			width: parent.width + radius
			anchors.left: parent.left
			anchors.leftMargin: button.TabBar.index === 0 ? 0 : -radius

			radius: 3
			color: button.checked ? Style.color.accent : (button.pressed ? Constants.lightgrey : Constants.white)
			border.color: Constants.blue
			border.width: 1
		}
	}
}
