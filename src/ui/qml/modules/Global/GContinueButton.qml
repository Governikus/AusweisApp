/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

GButton {
	Layout.alignment: Qt.AlignHCenter
	icon.source: "qrc:///images/material_arrow_right.svg"
	layoutDirection: Qt.RightToLeft

	//: LABEL ALL_PLATFORMS
	text: qsTr("Continue")
	tintIcon: true
	visible: text !== ""
}
