/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0

HistoryListItem {
	id: listItem

	width: parent.width

	contentMarginRight: deleteButton.width + 2 * Constants.groupbox_spacing

	Image {
		id: deleteButton

		anchors.right: parent.right
		anchors.verticalCenter: parent.verticalCenter
		anchors.margins: Constants.groupbox_spacing
		sourceSize.width: Style.dimens.small_icon_size

		source: "qrc:///images/trash_icon.svg"
		fillMode: Image.PreserveAspectFit
	}

	MouseArea {
		height: parent.height
		anchors.right: parent.right
		anchors.left: deleteButton.left
		anchors.leftMargin: - Constants.groupbox_spacing


		//: LABEL ANDROID
		Accessible.name: qsTr("Delete entry") + SettingsModel.translationTrigger
		Accessible.role: Accessible.Button
		onClicked: HistoryModel.removeRows(index, 1)
	}
}
