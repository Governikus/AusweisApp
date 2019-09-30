/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.HistoryModel 1.0

HistoryListItem {
	id: listItem

	width: parent.width

	contentMarginRight: deleteButton.width + 2 * Constants.groupbox_spacing

	showRightArrow: false

	Image {
		id: deleteButton
		sourceSize.width: Style.dimens.small_icon_size
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.margins: Constants.groupbox_spacing

		source: "qrc:///images/trash_icon.svg"
		fillMode: Image.PreserveAspectFit

		MouseArea {
			anchors.fill: parent

			onClicked: HistoryModel.removeRows(historyModelItem ? historyModelItem.index : -1, 1)
		}
	}
}
