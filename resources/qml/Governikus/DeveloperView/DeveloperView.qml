/*
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0


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

		Item {
			id: testUriContainer
			width: parent.usableWidth
			height: Math.max(testUriText.height, testUriSwitch.height)

			Item {
				id: testUriText
				height: testUriNameText.height + testUriDateText.height
				anchors.left: testUriContainer.left
				anchors.right: testUriSwitch.left
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: testUriContainer.verticalCenter

				GText {
					id: testUriNameText
					anchors.bottomMargin: 2
					//: LABEL ALL_PLATFORMS
					text: qsTr("Test environment") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_accent
				}

				GText {
					id: testUriDateText
					width: parent.width
					anchors.top: testUriNameText.bottom
					//: LABEL ALL_PLATFORMS
					text: qsTr("Use the test environment during a self-authentication") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_secondary
				}
			}

			GSwitch {
				id: testUriSwitch

				anchors.right: testUriContainer.right
				anchors.verticalCenter: testUriContainer.verticalCenter

				Accessible.name: qsTr("Test environment") + SettingsModel.translationTrigger

				initialState: SettingsModel.useSelfauthenticationTestUri
				onSwitched: SettingsModel.useSelfauthenticationTestUri = testUriSwitch.isOn
			}
		}

		Item {
			id: devModeContainer
			width: parent.usableWidth
			height: Math.max(devModeText.height, devModeSwitch.height)

			Item {
				id: devModeText
				height: devModeNameText.height + devModeDateText.height
				anchors.left: devModeContainer.left
				anchors.right: devModeSwitch.left
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: devModeContainer.verticalCenter

				GText {
					id: devModeNameText
					anchors.bottomMargin: 2
					//: LABEL ALL_PLATFORMS
					text: qsTr("Developer Mode") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_accent
				}

				GText {
					id: devModeDateText
					width: parent.width
					anchors.top: devModeNameText.bottom
					//: LABEL ALL_PLATFORMS
					text: qsTr("Use a more tolerant mode") + SettingsModel.translationTrigger
					textStyle: Style.text.normal_secondary
				}
			}

			GSwitch {
				id: devModeSwitch

				anchors.right: devModeContainer.right
				anchors.verticalCenter: devModeContainer.verticalCenter

				Accessible.name: qsTr("Developer mode") + SettingsModel.translationTrigger

				initialState: SettingsModel.developerMode
				onSwitched: SettingsModel.developerMode = devModeSwitch.isOn
			}
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
	}
}
