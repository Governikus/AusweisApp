/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


RowLayout {
	id: historyControls

	property bool showDeleteAll: true

	height: Style.dimens.titlebar_height

	spacing: Constants.component_spacing

	HistoryViewConfirmationPopup {
		id: deleteHistoryConfirmationPopup
	}

	GSwitch {
		id: enableHistorySwitch

		color: Constants.green
		initialState: SettingsModel.historyEnabled
		onSwitched: {
			SettingsModel.historyEnabled = enableHistorySwitch.isOn
			//: LABEL ANDROID IOS
			ApplicationModel.showFeedback((SettingsModel.historyEnabled ? qsTr("History enabled") : qsTr("History disabled")) + SettingsModel.translationTrigger)
		}
	}

	Image {
		id: deleteEntriesButtonImage

		visible: historyControls.showDeleteAll

		sourceSize.height: 36
		sourceSize.width: 36

		fillMode: Image.PreserveAspectFit
		source: "qrc:///images/trash_icon_white.svg"

		MouseArea {
			id: deleteEntriesButton

			anchors.fill: parent

			onClicked: deleteHistoryConfirmationPopup.open()
		}
	}
 }
