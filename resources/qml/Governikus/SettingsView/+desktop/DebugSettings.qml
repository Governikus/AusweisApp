/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
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
		contentPadding: 0
		spacing: Constants.component_spacing

		//: LABEL DESKTOP
		title: qsTr("Create dummy entries")

		RowLayout {
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			Layout.topMargin: Constants.pane_padding

			GButton {
				//: LABEL DESKTOP
				text: qsTr("Logfile")

				onClicked: {
					LogModel.saveDummyLogFile();
					ApplicationModel.showFeedback("Created new logfile.");
				}
			}
		}
		ColumnLayout {
			spacing: 0

			GSwitch {
				Layout.fillWidth: true
				checked: SettingsModel.showBetaTesting
				//: LABEL DESKTOP
				text: qsTr("Show beta testing image")

				onCheckedChanged: SettingsModel.showBetaTesting = checked
			}
			GSwitch {
				Layout.fillWidth: true
				checked: SettingsModel.enableCanAllowed
				//: LABEL DESKTOP
				text: qsTr("Support CAN allowed mode")

				onCheckedChanged: SettingsModel.enableCanAllowed = checked
			}
			GSwitch {
				Layout.fillWidth: true
				checked: SettingsModel.skipRightsOnCanAllowed
				enabled: SettingsModel.enableCanAllowed
				//: LABEL DESKTOP
				text: qsTr("Skip rights page in CAN allowed mode")

				onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
			}
		}
		GButton {
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			//: LABEL DESKTOP
			text: qsTr("Reset hideable dialogs")

			onClicked: {
				SettingsModel.resetHideableDialogs();
			}
		}
		GText {
			Layout.bottomMargin: Constants.pane_padding
			Layout.leftMargin: Constants.pane_padding
			Layout.rightMargin: Constants.pane_padding
			activeFocusOnTab: true
			//: LABEL DESKTOP
			text: qsTr("Show Transport PIN reminder, store feedback and close reminder dialogs.")

			FocusFrame {
			}
		}
	}
}
