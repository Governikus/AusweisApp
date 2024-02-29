/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar

Row {
	id: logControls

	property alias allowRemoveAll: removeAllButton.visible

	signal removeAll
	signal share(point popupPosition)

	spacing: Constants.component_spacing

	TitleBarAction {
		Accessible.name: qsTr("Share log")
		icon: "qrc:///images/mobile/share.svg"

		onClicked: logControls.share(mapToGlobal(width / 2, height))
	}
	TitleBarAction {
		id: removeAllButton

		Accessible.name: qsTr("Delete all logs")
		icon: "qrc:///images/trash_icon.svg"

		onClicked: logControls.removeAll()
	}
}
