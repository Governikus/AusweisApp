/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.SettingsModel 1.0


BaseHistoryView {
	id: baseItem

	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
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
					text: qsTr("Delete all") + SettingsModel.translationTrigger
				}
			}
		]

		onClicked: deleteHistoryConfirmationPopup.open()
	}

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}
}
