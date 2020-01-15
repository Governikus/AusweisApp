/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.View 1.0


BaseHistoryView {
	id: baseItem

	rightTitleBarAction: HistoryViewTitleBarControls {
		showDeleteAll: baseItem.historyItemCount !== 0
	}
}
