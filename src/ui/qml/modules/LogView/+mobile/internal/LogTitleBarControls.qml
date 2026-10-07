/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

import QtQuick

import Governikus.TitleBar

Row {
	id: root

	property alias showFilter: filterButton.visible
	property alias showRemove: removeAllButton.visible
	property alias showShare: shareButton.visible

	signal filterClicked
	signal removeAllClicked
	signal shareClicked(point popupPosition)

	spacing: 0

	TitleBarAction {
		id: filterButton

		//: MOBILE
		Accessible.name: qsTr("Filter")
		icon.source: "qrc:///images/filter.svg"
		visible: false

		onClicked: root.filterClicked()
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
