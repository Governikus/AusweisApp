/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.View 1.0
import Governikus.Style 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.SettingsModel 1.0

ColumnLayout {

	spacing: Constants.component_spacing

	GText {
		//: LABEL DESKTOP_QML
		text: qsTr("Create dummy entries") + SettingsModel.translationTrigger
		textStyle: Style.text.header_accent
	}

	RowLayout {
		GButton {
			//: LABEL DESKTOP_QML
			text: qsTr("Logfile") + SettingsModel.translationTrigger
			onClicked: {
				LogModel.saveDummyLogfile()
				ApplicationModel.showFeedback("Created new logfile.")
			}
		}
		GButton {
			//: LABEL DESKTOP_QML
			text: qsTr("History") + SettingsModel.translationTrigger
			onClicked: {
				HistoryModel.createDummyEntry()
				ApplicationModel.showFeedback("Created new history entry.")
			}
		}
	}

	ToggleableOption {
		//: LABEL DESKTOP_QML
		text: qsTr("Show beta testing image") + SettingsModel.translationTrigger
		checked: SettingsModel.showBetaTesting
		onCheckedChanged: SettingsModel.showBetaTesting = checked
	}
}
