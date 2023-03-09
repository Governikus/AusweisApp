/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0

GButton {
	id: root

	property bool iconVisible: true

	background: null
	horizontalPadding: 0
	icon.source: iconVisible ? "qrc:///images/info.svg" : ""
	maximumLineCount: 2
	//: LABEL ALL_PLATFORMS
	text: qsTr("More information")
	textStyle: Style.text.link_accent
	tintIcon: true
	verticalPadding: 2
}
