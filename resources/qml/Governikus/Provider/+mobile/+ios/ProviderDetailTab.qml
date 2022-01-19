/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TabButton {
	id: button

	Accessible.role: Accessible.PageTab
	Accessible.name: text

	contentItem: GText {
		Accessible.ignored: true

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
			color: button.checked ? Style.color.accent : (button.pressed ?  Style.color.background_pane_active : Style.color.background_pane)
			border.color: Constants.blue
			border.width: 1
		}
	}
}
