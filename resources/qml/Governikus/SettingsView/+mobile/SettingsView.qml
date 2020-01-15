/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0

SectionPage {
	id: baseItem

	navigationAction: NavigationAction { state: topLevelPage ? "" : "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Settings") + SettingsModel.translationTrigger

	content: Column {
		id: mainColumn

		readonly property int usableWidth: width - 2 * padding

		width: baseItem.width
		padding: Constants.pane_padding
		spacing: Constants.component_spacing


		TitledSeparator {
			width: parent.usableWidth
			topPadding: 0
			//: LABEL ANDROID IOS
			title: qsTr("General") + SettingsModel.translationTrigger
		}

		SettingsEntry {
			width: parent.usableWidth
			//: LABEL ANDROID IOS
			title: qsTr("Change language") + SettingsModel.translationTrigger
			icon: "qrc:///images/location_flag_%1.svg".arg(SettingsModel.language)
			tintIcon: false
			description: {
				switch(SettingsModel.language) {
					case "de":
						return "Deutsch"
					default:
						return "English"
				}
			}

			onClicked: popup.open()

			LanguageSelectionPopup {
				id: popup
			}
		}

		TitledSeparator {
			width: parent.usableWidth
			//: LABEL ANDROID IOS
			title: qsTr("Smartphone as card reader") + SettingsModel.translationTrigger
		}

		Item {
			id: serverNameBase

			height: serverNameText.height + serverName.height
			width: parent.usableWidth

			GText {
				id: serverNameText

				width: parent.width

				//: LABEL ANDROID IOS
				text: qsTr("Device name") + SettingsModel.translationTrigger
				textStyle: Style.text.normal_accent
			}

			GTextField {
				id: serverName

				function saveInput() {
					focus = false
					SettingsModel.serverName = text
					text = SettingsModel.serverName
				}

				width: parent.width
				anchors.top: serverNameText.bottom
				anchors.topMargin: 2

				Component.onCompleted: text = SettingsModel.serverName

				maximumLength: Constants.maximumDeviceNameLength
				onAccepted: saveInput()
				onFocusChanged: if (!focus) saveInput()
				onVisibleChanged: if (!visible) saveInput()
			}
		}

		LabeledSwitch {
			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("PIN pad mode") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Enter PIN on this device") + SettingsModel.translationTrigger
			checked: SettingsModel.pinPadMode
			onCheckedChanged: SettingsModel.pinPadMode = checked
		}

		SettingsEntry {
			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("Remote card reader") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Click here to use another smartphone as a card reader") + SettingsModel.translationTrigger
			onClicked: baseItem.firePush(remoteServiceSettings)

			Component {
				id: remoteServiceSettings
				RemoteServiceSettings {
					Component.onCompleted: RemoteServiceModel.detectRemoteDevices = true
					Component.onDestruction: RemoteServiceModel.detectRemoteDevices = false
				}
			}
		}

		TitledSeparator {
			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("Security and privacy") + SettingsModel.translationTrigger
		}

		LabeledSwitch {
			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("Shuffle keypad buttons") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Randomize the order of the on screen keypad buttons") + SettingsModel.translationTrigger

			checked: SettingsModel.shuffleScreenKeyboard
			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}

		LabeledSwitch {
			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("Privacy keypad") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Avoid visual feedback in the on screen keypad") + SettingsModel.translationTrigger
			checked: SettingsModel.visualPrivacy
			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}

		LabeledSwitch {
			width: parent.usableWidth

			//: LABEL ANDROID IOS
			title: qsTr("History") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Save authentification history") + SettingsModel.translationTrigger
			checked: SettingsModel.historyEnabled
			onCheckedChanged: SettingsModel.historyEnabled = checked
		}
	}
}
