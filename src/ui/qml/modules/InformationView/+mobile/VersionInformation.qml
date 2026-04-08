/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.TitleBar
import Governikus.View
import Governikus.Style
import Governikus.Type

FlickableSectionPage {
	id: root

	//: MOBILE %1 is replaced with the application name
	title: qsTr("%1 version").arg(Qt.application.name)

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.pop()
	}

	MouseArea {
		property int advancedSettingsCounter: 0

		Layout.fillWidth: true
		implicitHeight: repeaterLayout.implicitHeight

		onClicked: {
			advancedSettingsCounter += 1;
			switch (advancedSettingsCounter) {
			case 7:
			case 8:
			case 9:
				if (!ApplicationModel.screenReaderRunning) {
					//: MOBILE Used in notifications when the user taps the version information
					ApplicationModel.showFeedback(qsTr("%1 more taps to toggle the advanced settings.").arg(10 - advancedSettingsCounter), true);
				}
				break;
			case 10:
				SettingsModel.advancedSettings = !SettingsModel.advancedSettings;
				ApplicationModel.showFeedback((SettingsModel.advancedSettings ?
					//: MOBILE
					qsTr("Advanced settings activated.") :
					//: MOBILE
					qsTr("Advanced settings deactivated.")), true);
				advancedSettingsCounter = 0;
				break;
			}
		}

		ColumnLayout {
			id: repeaterLayout

			spacing: Style.dimens.pane_spacing

			anchors {
				left: parent.left
				right: parent.right
			}
			Repeater {
				model: VersionInformationModel

				LabeledText {
					required property string key
					required property string value

					Layout.fillWidth: true
					label: key
					text: value

					onFocusChanged: if (focus)
						root.positionViewAtItem(this)
				}
			}
		}
	}
}
