/*
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.HistoryModel 1.0

SwipeActionDelegate {
	id: swipeComponent

	property var historyModelItem
	property bool showSeparator: true

	actionColor: Constants.red
	actionIcon: "qrc:///images/material_delete.svg"
	actionIconTintColor: Style.color.button_text
	//: INFO IOS Accessible name for the trash icon of a history entry.
	actionAccessibleName: qsTr("Delete history entry: %1").arg(historyModelItem.subject)

	contentItem: HistoryListItem {
		historyModelItem: swipeComponent.historyModelItem
		mouseAreaEnabled: false
		pressed: swipeComponent.pressed
		showSeparator: swipeComponent.showSeparator
	}

	onActionClicked: HistoryModel.removeRows(index, 1)
}
