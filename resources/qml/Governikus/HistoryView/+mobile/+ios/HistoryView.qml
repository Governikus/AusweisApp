/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0

BaseHistoryView {
	id: baseItem

	rightTitleBarAction: TitleBarAction {
		id: rightAction

		states: [
			State {
				name: "none"
				when: baseItem.historyItemCount === 0
				PropertyChanges { target: rightAction; text: "" }
			},
			State {
				name: "deleteAll"
				when: baseItem.historyItemCount !== 0
				PropertyChanges {
					target: rightAction
					//: LABEL IOS
					text: qsTr("Delete all entries")
				}
			}
		]

		onClicked: deleteHistoryConfirmationPopup.open()
	}

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}
}
