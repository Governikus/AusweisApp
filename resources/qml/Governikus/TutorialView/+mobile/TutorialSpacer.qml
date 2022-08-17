/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	id: baseItem

	property alias text: textContent.text
	height: textContent.height + 2 * Constants.component_spacing

	GText {
		id: textContent
		anchors.centerIn: parent
		color: Constants.white
		width: parent.width * 0.8

		Accessible.onScrollDownAction: baseItem.Accessible.scrollDownAction()
		Accessible.onScrollUpAction: baseItem.Accessible.scrollUpAction()

		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}
}
