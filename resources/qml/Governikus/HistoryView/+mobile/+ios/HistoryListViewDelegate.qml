/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0

SwipeActionDelegate {
	id: swipeComponent

	property var historyModelItem
	property bool showSeparator: true

	actionColor: Constants.red
	actionIcon: "qrc:///images/trash_icon_white.svg"
	//: INFO IOS Accessible name for the trash icon of a history entry.
	actionAccessibleName: qsTr("Delete history entry: %1").arg(historyModelItem.subject) + SettingsModel.translationTrigger

	contentItem: HistoryListItem {
		historyModelItem: swipeComponent.historyModelItem
		mouseAreaEnabled: false
		pressed: swipeComponent.pressed
		showSeparator: swipeComponent.showSeparator
	}

	onActionClicked: HistoryModel.removeRows(index, 1)
}
