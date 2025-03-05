/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtQuick.Controls

import Governikus.Global
import Governikus.Type

Item {
	id: root

	property ConfirmationPopup dominationPopup: null

	Connections {
		function onFireDominatorChanged() {
			if (root.dominationPopup) {
				root.dominationPopup.close();
				root.dominationPopup.destroy();
				root.dominationPopup = null;
			}
			if (UiPluginModel.dominated) {
				root.dominationPopup = domination.createObject(root, {
					text: UiPluginModel.dominator
				});
				root.dominationPopup.open();
			}
		}

		target: UiPluginModel
	}
	Component {
		id: domination

		ConfirmationPopup {
			closePolicy: Popup.NoAutoClose
			mainTextFormat: Text.PlainText
			style: ConfirmationPopup.PopupStyle.NoButtons
			//: INFO DESKTOP The AA is currently remote controlled via the SDK interface, concurrent usage of the AA is not possible.
			title: qsTr("Another application uses %1").arg(Qt.application.name)
		}
	}
}
