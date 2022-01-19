/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick.Layouts 1.12

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
		//: LABEL DESKTOP
		text: qsTr("Create dummy entries")
		textStyle: Style.text.header_accent
	}

	RowLayout {
		GButton {
			//: LABEL DESKTOP
			text: qsTr("Logfile")
			onClicked: {
				LogModel.saveDummyLogFile()
				ApplicationModel.showFeedback("Created new logfile.")
			}
		}
		GButton {
			//: LABEL DESKTOP
			text: qsTr("History")
			onClicked: {
				HistoryModel.createDummyEntry()
				ApplicationModel.showFeedback("Created new history entry.")
			}
		}
	}

	ToggleableOption {
		//: LABEL DESKTOP
		text: qsTr("Show beta testing image")
		checked: SettingsModel.showBetaTesting
		onCheckedChanged: SettingsModel.showBetaTesting = checked
	}

	ToggleableOption {
		//: LABEL DESKTOP
		text: qsTr("Support CAN allowed mode")
		checked: SettingsModel.enableCanAllowed
		onCheckedChanged: SettingsModel.enableCanAllowed = checked
	}

	ToggleableOption {
		//: LABEL DESKTOP
		text: qsTr("Skip rights page in CAN allowed mode")
		enabled: SettingsModel.enableCanAllowed
		checked: SettingsModel.skipRightsOnCanAllowed
		onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
	}
}
