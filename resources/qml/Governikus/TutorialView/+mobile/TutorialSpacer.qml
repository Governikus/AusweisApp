/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0

Rectangle {
	property alias text: textContent.text
	height: textContent.height + 2 * Constants.component_spacing

	GText {
		id: textContent
		anchors.centerIn: parent
		color: Constants.white
		width: parent.width * 0.8
		textStyle: Style.text.tutorial_header
		horizontalAlignment: Text.AlignHCenter
	}
}
