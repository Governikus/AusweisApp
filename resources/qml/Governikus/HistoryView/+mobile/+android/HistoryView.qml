/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.View 1.0


BaseHistoryView {
	id: baseItem

	rightTitleBarAction: HistoryViewTitleBarControls {
		showDeleteAll: baseItem.historyItemCount !== 0
	}
}
