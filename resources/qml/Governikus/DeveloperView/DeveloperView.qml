import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0


SectionPage {
	id: root
	leftTitleBarAction: TitleBarAction { state: !topLevelPage ? "back" : ""; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Developer options") + settingsModel.translationTrigger; font.bold: true }

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
				Text {
					id: testUriNameText
					anchors.bottomMargin: Utils.dp(2)
					font.pixelSize: Utils.dp(16)
					color: Constants.secondary_text
					opacity: 0.87
					text: qsTr("Test environment") + settingsModel.translationTrigger
				}

				Text {
					id: testUriDateText
					width: parent.width
					anchors.top: testUriNameText.bottom
					font.pixelSize: Utils.dp(14)
					color: Constants.secondary_text
					opacity: 0.38
					text: qsTr("Use the test environment during a selfauthentication") + settingsModel.translationTrigger
					wrapMode: Text.WordWrap
				}
			}

			GSwitch {
				id: testUriSwitch
				anchors.right: testUriContainer.right
				anchors.verticalCenter: testUriContainer.verticalCenter
				initialState: settingsModel.useSelfauthenticationTestUri
				onSwitched: settingsModel.useSelfauthenticationTestUri = testUriSwitch.isOn
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
				Text {
					id: devModeNameText
					anchors.bottomMargin: Utils.dp(2)
					font.pixelSize: Utils.dp(16)
					color: Constants.secondary_text
					opacity: 0.87
					text: qsTr("Developer Mode") + settingsModel.translationTrigger
				}

				Text {
					id: devModeDateText
					width: parent.width
					anchors.top: devModeNameText.bottom
					font.pixelSize: Utils.dp(14)
					color: Constants.secondary_text
					opacity: 0.38
					text: qsTr("Use a more tolerant mode") + settingsModel.translationTrigger
					wrapMode: Text.WordWrap
				}
			}

			GSwitch {
				id: devModeSwitch
				anchors.right: devModeContainer.right
				anchors.verticalCenter: devModeContainer.verticalCenter
				initialState: settingsModel.developerMode
				onSwitched: settingsModel.developerMode = devModeSwitch.isOn
			}
		}

		Text {
			text: qsTr("Change the layout style") + settingsModel.translationTrigger
			font.pixelSize: Constants.normal_font_size
			font.bold: true
			color: Constants.blue
		}

		GRadioButton {
			text: qsTr("iOS") + settingsModel.translationTrigger
			checked: plugin.platformStyle === text.toLowerCase()
			onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
		}

		GRadioButton {
			text: qsTr("Android") + settingsModel.translationTrigger
			checked: plugin.platformStyle === text.toLowerCase()
			onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
		}

		GRadioButton {
			text: qsTr("Tablet, Android") + settingsModel.translationTrigger
			checked: plugin.platformStyle === text.toLowerCase()
			onCheckedChanged: if (checked) { plugin.applyPlatformStyle(text.toLowerCase()) }
		}
	}
}
