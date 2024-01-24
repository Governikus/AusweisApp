/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick.Layouts
import Governikus.Global
import Governikus.View
import Governikus.Style
import Governikus.Type.ApplicationModel
import Governikus.Type.LogModel
import Governikus.Type.SettingsModel

ColumnLayout {
	spacing: Constants.component_spacing

	GPane {
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		//: LABEL DESKTOP
		title: qsTr("Create dummy entries")

		RowLayout {
			GButton {
				//: LABEL DESKTOP
				text: qsTr("Logfile")

				onClicked: {
					LogModel.saveDummyLogFile();
					ApplicationModel.showFeedback("Created new logfile.");
				}
			}
		}
		LabeledSwitch {
			checked: SettingsModel.showBetaTesting
			//: LABEL DESKTOP
			title: qsTr("Show beta testing image")

			onCheckedChanged: SettingsModel.showBetaTesting = checked
		}
		LabeledSwitch {
			checked: SettingsModel.enableCanAllowed
			//: LABEL DESKTOP
			title: qsTr("Support CAN allowed mode")

			onCheckedChanged: SettingsModel.enableCanAllowed = checked
		}
		LabeledSwitch {
			checked: SettingsModel.skipRightsOnCanAllowed
			enabled: SettingsModel.enableCanAllowed
			//: LABEL DESKTOP
			title: qsTr("Skip rights page in CAN allowed mode")

			onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
		}
		GButton {
			//: LABEL DESKTOP
			text: qsTr("Reset hideable dialogs")

			onClicked: {
				SettingsModel.resetHideableDialogs();
			}
		}
		GText {
			activeFocusOnTab: true
			//: LABEL DESKTOP
			text: qsTr("Show Transport PIN reminder, store feedback and close reminder dialogs.")

			FocusFrame {
			}
		}
	}
}
