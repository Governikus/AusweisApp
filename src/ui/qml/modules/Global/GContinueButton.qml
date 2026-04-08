/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts
import Governikus.Style

GButton {
	Layout.alignment: Style.scanPatternAlignment
	icon.source: "qrc:///images/material_arrow_right.svg"
	layoutDirection: Qt.RightToLeft

	//: ALL_PLATFORMS
	text: qsTr("Continue")
	tintIcon: true
	visible: text !== ""
}
