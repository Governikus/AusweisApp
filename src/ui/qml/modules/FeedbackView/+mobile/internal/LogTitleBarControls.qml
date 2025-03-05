/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.TitleBar
import Governikus.Style

Row {
	id: root

	property alias allowRemoveAll: removeAllButton.visible

	signal removeAll
	signal share(point popupPosition)

	spacing: Style.dimens.pane_spacing

	TitleBarAction {
		Accessible.name: qsTr("Share log")
		icon.source: "qrc:///images/mobile/share.svg"

		onClicked: root.share(mapToGlobal(width / 2, height))
	}
	TitleBarAction {
		id: removeAllButton

		Accessible.name: qsTr("Delete all logs")
		icon.source: "qrc:///images/trash_icon.svg"

		onClicked: root.removeAll()
	}
}
