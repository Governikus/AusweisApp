/*
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0

GText {
	id: root
	signal clicked

	Accessible.role: Accessible.Button
	activeFocusOnTab: true
	font.underline: true

	//: LABEL ALL_PLATFORMS
	text: qsTr("More information")
	textStyle: Constants.is_desktop ? Style.text.header : Style.text.normal_accent

	Accessible.onPressAction: root.clicked()
	Keys.onSpacePressed: root.clicked()

	MouseArea {
		anchors.fill: parent
		anchors.margins: -12
		cursorShape: Qt.PointingHandCursor

		onClicked: root.clicked()
	}
	FocusFrame {
	}
}
