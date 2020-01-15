/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.SettingsView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.ApplicationModel 1.0


SectionPage {
	id: root
	navigationAction: NavigationAction { state: !topLevelPage ? "back" : ""; onClicked: firePop() }
	//: LABEL ALL_PLATFORMS
	title: qsTr("Developer options") + SettingsModel.translationTrigger

	content: Column {
		id: mainColumn
		width: root.width
		padding: Constants.pane_padding
		spacing: Constants.component_spacing

		readonly property int usableWidth: width - 2 * padding

		LabeledSwitch {
			id: testUriSwitch

			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("Testmode for the self-authentication") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Use the test environment during a self-authentication") + SettingsModel.translationTrigger
			checked: SettingsModel.useSelfauthenticationTestUri
			onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = testUriSwitch.checked
		}

		LabeledSwitch {
			id: devModeSwitch

			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("Developer Mode") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Use a more tolerant mode") + SettingsModel.translationTrigger
			checked: SettingsModel.developerMode
			onCheckedChanged: SettingsModel.developerMode = devModeSwitch.checked
		}

		GText {
			//: LABEL ALL_PLATFORMS
			text: qsTr("Change the layout style") + SettingsModel.translationTrigger
			textStyle: Style.text.normal_accent
		}

		GRadioButton {
			//: LABEL ALL_PLATFORMS
			text: qsTr("iOS") + SettingsModel.translationTrigger
			checked: plugin.platformStyle === text.toLowerCase()
			onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
		}

		GRadioButton {
			//: LABEL ALL_PLATFORMS
			text: qsTr("Android") + SettingsModel.translationTrigger
			checked: plugin.platformStyle === text.toLowerCase()
			onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
		}

		GRadioButton {
			//: LABEL ALL_PLATFORMS
			text: qsTr("Tablet, Android") + SettingsModel.translationTrigger
			checked: plugin.platformStyle === text.toLowerCase()
			onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
		}

		GText {
			//: LABEL ALL_PLATFORMS
			text: qsTr("Create dummy entries") + SettingsModel.translationTrigger
			textStyle: Style.text.normal_accent
		}

		GButton {
			//: LABEL ALL_PLATFORMS
			text: qsTr("Logfile") + SettingsModel.translationTrigger
			onClicked: {
				LogModel.saveDummyLogfile()
				ApplicationModel.showFeedback("Created new logfile.")
			}
		}

		GButton {
			//: LABEL ALL_PLATFORMS
			text: qsTr("History") + SettingsModel.translationTrigger
			onClicked: {
				HistoryModel.createDummyEntry()
				ApplicationModel.showFeedback("Created new history entry.")
			}
		}
	}
}
