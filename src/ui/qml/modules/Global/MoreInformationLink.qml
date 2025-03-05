/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style

GLink {
	id: root

	property bool iconVisible: true

	Layout.fillWidth: true
	font.underline: true
	horizontalPadding: 0
	icon.source: iconVisible ? "qrc:///images/info.svg" : ""
	iconSize: Style.dimens.small_icon_size
	maximumLineCount: 2
	//: LABEL ALL_PLATFORMS
	text: qsTr("More information")
	textStyle: Style.text.link
	tintIcon: true
	verticalPadding: 2
}
