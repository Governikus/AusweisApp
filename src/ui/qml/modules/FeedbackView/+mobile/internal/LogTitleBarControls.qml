/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick

import Governikus.TitleBar
import Governikus.Style

Row {
	id: root

	property alias filter: filterButton.filter
	property alias showFilter: filterButton.visible
	property alias showRemove: removeAllButton.visible
	property alias showShare: shareButton.visible

	signal removeAllClicked
	signal shareClicked(point popupPosition)

	spacing: Style.dimens.pane_spacing

	TitleBarAction {
		id: filterButton

		property bool filter: false

		Accessible.checked: filter
		//: MOBILE
		Accessible.name: qsTr("Filter")
		Accessible.role: Accessible.CheckBox
		icon.source: filter ? "qrc:///images/filter_off.svg" : "qrc:///images/filter.svg"
		visible: false

		onClicked: filter = !filter
	}
	TitleBarAction {
		id: shareButton

		//: MOBILE
		Accessible.name: qsTr("Share log")
		icon.source: "qrc:///images/mobile/share.svg"
		visible: false

		onClicked: root.shareClicked(mapToGlobal(width / 2, height))
	}
	TitleBarAction {
		id: removeAllButton

		//: MOBILE
		Accessible.name: qsTr("Delete all logs")
		icon.source: "qrc:///images/trash_icon.svg"
		visible: false

		onClicked: root.removeAllClicked()
	}
}
