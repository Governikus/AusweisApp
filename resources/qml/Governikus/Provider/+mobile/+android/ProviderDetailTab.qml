/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TabButton {
	id: button
	padding: 10

	Accessible.name: text

	contentItem: GText {
		Accessible.ignored: true

		text: button.text
		textStyle: button.checked || button.pressed ? Style.text.hint_accent : Style.text.hint
		elide: Text.ElideRight
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}

	background: Rectangle {
		color: Style.color.background
		clip: true

		Rectangle {
			height: parent.height + radius
			width: parent.width

			radius: 3
			color: button.checked ? Style.color.background_pane : Style.color.border
			border.color: Style.color.border
			border.width: Style.dimens.separator_size
		}
	}
}
