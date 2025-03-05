/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	//: LABEL ANDROID IOS
	title: qsTr("Version Information")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	MouseArea {
		property int advancedSettingsCounter: 0

		Layout.fillWidth: true
		implicitHeight: pane.implicitHeight

		onClicked: {
			advancedSettingsCounter += 1;
			switch (advancedSettingsCounter) {
			case 7:
			case 8:
			case 9:
				if (!ApplicationModel.isScreenReaderRunning) {
					//: INFO ANDROID IOS Used in notifications when the user taps the version information
					ApplicationModel.showFeedback(qsTr("%1 more presses to toggle the advanced settings.").arg(10 - advancedSettingsCounter), true);
				}
				break;
			case 10:
				SettingsModel.advancedSettings = !SettingsModel.advancedSettings;
				ApplicationModel.showFeedback((SettingsModel.advancedSettings ?
					//: LABEL ANDROID IOS
					qsTr("Advanced settings activated.") :
					//: LABEL ANDROID IOS
					qsTr("Advanced settings deactivated.")), true);
				advancedSettingsCounter = 0;
				break;
			}
		}

		GPane {
			id: pane

			anchors {
				left: parent.left
				right: parent.right
			}
			Repeater {
				model: VersionInformationModel

				LabeledText {
					required property string key
					required property string value

					anchors.left: parent.left
					anchors.right: parent.right
					label: key
					text: value
				}
			}
		}
	}
}
