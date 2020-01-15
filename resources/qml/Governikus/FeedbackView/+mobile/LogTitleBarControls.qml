/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0

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

	Image {
		height: Style.dimens.small_icon_size
		width: height

		Accessible.role: Accessible.Button
		Accessible.name: qsTr("Share logfile") + SettingsModel.translationTrigger
		Accessible.onPressAction: if (Qt.platform.os === "ios") mouseAreaShare.clicked(null)

		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/share.svg"

		MouseArea {
			id: mouseAreaShare

			anchors.fill: parent
			anchors.margins: -8

			onClicked: logControls.share(mapToGlobal(width / 2, height))
		}
	}

	Image {
		id: removeButton

		height: Style.dimens.small_icon_size
		width: height

		Accessible.role: Accessible.Button
		Accessible.name: qsTr("Delete logfile") + SettingsModel.translationTrigger
		Accessible.onPressAction: if (Qt.platform.os === "ios") mouseAreaRemove.clicked(null)

		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/trash_icon_white.svg"

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
		Accessible.name: qsTr("Delete all logfiles") + SettingsModel.translationTrigger
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
