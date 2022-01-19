/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0

Row {
	id: logControls

	signal share(point popupPosition)
	signal remove()
	signal removeAll()

	readonly property int contentWidth: width
	property alias allowRemove: removeButton.visible
	property alias allowRemoveAll: removeAllButton.visible

	spacing: 18
	anchors.verticalCenter: parent ? parent.verticalCenter : undefined

	ShareButton {
		Accessible.name: qsTr("Share log")

		onClicked: logControls.share(mapToGlobal(width / 2, height))
	}

	TintableIcon {
		id: removeButton

		height: Style.dimens.small_icon_size

		Accessible.role: Accessible.Button
		Accessible.name: qsTr("Delete log")
		Accessible.onPressAction: if (Qt.platform.os === "ios") mouseAreaRemove.clicked(null)

		tintColor: Style.color.button_text
		sourceSize.height: height
		source: "qrc:///images/material_delete.svg"

		MouseArea {
			id: mouseAreaRemove

			anchors.fill: parent
			anchors.margins: -8

			onClicked: logControls.remove()
		}
	}

	Image {
		id: removeAllButton

		height: Style.dimens.small_icon_size
		width: height

		Accessible.role: Accessible.Button
		Accessible.name: qsTr("Delete all logs")
		Accessible.onPressAction: if (Qt.platform.os === "ios") mouseAreaRemove.clicked(null)

		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/trash_icon_all.svg"

		MouseArea {
			id: mouseAreaRemoveAll

			anchors.fill: parent
			anchors.margins: -8

			onClicked: logControls.removeAll()
		}
	}
 }
