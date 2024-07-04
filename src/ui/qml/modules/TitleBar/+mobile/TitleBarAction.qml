/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.Style
import Governikus.View

GLink {
	id: baseItem

	Accessible.focusable: true
	Accessible.ignored: icon.source == "" && text === ""
	Accessible.name: text
	Accessible.role: Accessible.Button
	activeFocusOnTab: !Accessible.ignored
	colorStyle: Style.color.linkTitle
	horizontalPadding: 0
	textStyle: Style.text.navigation

	Keys.onSpacePressed: clicked()

	MouseArea {
		id: mouseArea

		anchors.fill: parent

		onClicked: baseItem.clicked()
	}
}
