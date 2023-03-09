/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0

Row {
	id: logControls

	property alias allowRemove: removeButton.visible
	property alias allowRemoveAll: removeAllButton.visible
	readonly property int contentWidth: width

	signal remove
	signal removeAll
	signal share(point popupPosition)

	anchors.verticalCenter: parent ? parent.verticalCenter : undefined
	spacing: 18

	TitleBarButton {
		Accessible.name: qsTr("Share log")
		source: "qrc:///images/mobile/platform_specific_share.svg"

		onClicked: logControls.share(mapToGlobal(width / 2, height))
	}
	TitleBarButton {
		id: removeButton
		Accessible.name: qsTr("Delete log")
		source: "qrc:///images/material_delete.svg"

		onClicked: logControls.remove()
	}
	TitleBarButton {
		id: removeAllButton
		Accessible.name: qsTr("Delete all logs")
		source: "qrc:///images/trash_icon_all.svg"

		onClicked: logControls.removeAll()
	}
}
