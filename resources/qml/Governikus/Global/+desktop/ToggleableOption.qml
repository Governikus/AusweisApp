/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0


GCheckBox {
	id: root

	activeFocusOnTab: true
	Accessible.name: text
	Accessible.role: Accessible.CheckBox

	textStyle: enabled ? Style.text.normal : Style.text.normal_secondary

	FocusFrame {}

	MouseArea {
		anchors.fill: parent
		onClicked: root.toggle()
		cursorShape: Qt.PointingHandCursor
	}
}
