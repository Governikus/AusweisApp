/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar

Row {
	id: logControls

	property alias allowRemove: removeButton.visible
	property alias allowRemoveAll: removeAllButton.visible

	signal remove
	signal removeAll
	signal share(point popupPosition)

	spacing: Constants.component_spacing

	TitleBarAction {
		Accessible.name: qsTr("Share log")
		icon: "qrc:///images/mobile/share.svg"

		onClicked: logControls.share(mapToGlobal(width / 2, height))
	}
	TitleBarAction {
		id: removeButton

		Accessible.name: qsTr("Delete log")
		icon: "qrc:///images/trash_icon.svg"

		onClicked: logControls.remove()
	}
	TitleBarAction {
		id: removeAllButton

		Accessible.name: qsTr("Delete all logs")
		icon: "qrc:///images/trash_icon_old.svg"

		onClicked: logControls.removeAll()
	}
}
