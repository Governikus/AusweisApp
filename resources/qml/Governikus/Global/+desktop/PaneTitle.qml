/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Style 1.0

GText {
	property alias checkBox: optionState

	visible: text !== ""

	textStyle: Style.text.header_accent

	GCheckBox {
		id: optionState

		visible: false

		anchors.verticalCenter: parent.verticalCenter
		anchors.right: parent.right

		activeFocusOnTab: false
	}
	MouseArea {
		anchors.fill: parent

		onReleased: if (mouse.button === Qt.LeftButton) optionState.toggle()
		cursorShape: optionState.visible ? Qt.PointingHandCursor : Qt.ArrowCursor
	}
}
