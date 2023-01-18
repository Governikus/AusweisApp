/*
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0

BaseHistoryView {
	id: baseItem
	rightTitleBarAction: TitleBarAction {
		//: LABEL IOS
		text: baseItem.historyItemCount > 0 ? qsTr("Delete all entries") : ""

		onClicked: deleteHistoryConfirmationPopup.open()
	}

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}
}
