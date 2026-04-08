/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.Global
import Governikus.Style

GLink {
	id: root

	Accessible.focusable: true
	Accessible.ignored: icon.source.toString() === "" && text === ""
	Accessible.name: text
	Accessible.role: Accessible.Button
	activeFocusOnTab: !Accessible.ignored
	colorStyle: Style.color.linkTitle
	horizontalPadding: 0
	textStyle: Style.text.navigation

	MouseArea {
		anchors.fill: parent

		onClicked: root.clicked()
	}
}
