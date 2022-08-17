/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0

GText {
	id: root

	signal clicked()

	Accessible.role: Accessible.Button
	Accessible.onPressAction: root.clicked()

	Keys.onSpacePressed: root.clicked()

	//: LABEL ALL_PLATFORMS
	text: qsTr("More information")
	textStyle: Constants.is_desktop ? Style.text.header : Style.text.normal_accent
	font.underline: true
	activeFocusOnTab: true

	MouseArea {
		anchors.fill: parent
		anchors.margins: -12

		onClicked: root.clicked()
		cursorShape: Qt.PointingHandCursor
	}

	FocusFrame {}
}
