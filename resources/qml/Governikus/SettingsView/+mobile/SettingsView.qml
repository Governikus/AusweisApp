/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.3

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

		SettingsEntry {
			visible: Constants.is_layout_android
			width: parent.usableWidth
			//: LABEL ANDROID
			title: qsTr("Screen orientation") + SettingsModel.translationTrigger
			description: (Screen.primaryOrientation === Qt.LandscapeOrientation
				//: LABEL ANDROID
				? qsTr("Landscape")
				//: LABEL ANDROID
				: qsTr("Portrait")
			) + SettingsModel.translationTrigger
			icon: Screen.primaryOrientation === Qt.LandscapeOrientation ? "qrc:///images/android/stay_primary_landscape-24px.svg" : "qrc:///images/android/stay_primary_portrait-24px.svg"
			tintIcon: true

			onClicked: orientationPopup.open()

			ScreenOrientationSelectionPopup {
				id: orientationPopup
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
			description: qsTr("Configure remote service for another device") + SettingsModel.translationTrigger
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
			title: qsTr("History") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Save authentication history") + SettingsModel.translationTrigger
			checked: SettingsModel.historyEnabled
			onCheckedChanged: SettingsModel.historyEnabled = checked
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
			title: qsTr("Keypad animations") + SettingsModel.translationTrigger
			//: LABEL ANDROID IOS
			description: qsTr("Visual feedback when pressing keypad buttons") + SettingsModel.translationTrigger
			checked: !SettingsModel.visualPrivacy
			onCheckedChanged: SettingsModel.visualPrivacy = !checked
		}

		Column {
			visible: SettingsModel.advancedSettings
			width: parent.usableWidth

			spacing: parent.spacing

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("CAN allowed mode") + SettingsModel.translationTrigger
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Support CAN allowed mode") + SettingsModel.translationTrigger
				//: LABEL ANDROID IOS
				description: qsTr("Allow the id card to be used with only the CAN") + SettingsModel.translationTrigger
				checked: SettingsModel.enableCanAllowed
				onCheckedChanged: SettingsModel.enableCanAllowed = checked
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Skip rights page") + SettingsModel.translationTrigger
				//: LABEL ANDROID IOS
				description: qsTr("Do not show the rights page, when in can allowed mode") + SettingsModel.translationTrigger
				enabled: SettingsModel.enableCanAllowed
				checked: SettingsModel.skipRightsOnCanAllowed
				onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
			}
		}

		Column {
			visible: plugin.debugBuild
			width: parent.usableWidth

			spacing: parent.spacing

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Developer Options") + SettingsModel.translationTrigger
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Testmode for the self-authentication") + SettingsModel.translationTrigger
				//: LABEL ANDROID IOS
				description: qsTr("Use the test environment during a self-authentication") + SettingsModel.translationTrigger

				checked: SettingsModel.useSelfauthenticationTestUri
				onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Developer Mode") + SettingsModel.translationTrigger
				//: LABEL ANDROID IOS
				description: qsTr("Use a more tolerant mode") + SettingsModel.translationTrigger
				checked: SettingsModel.developerMode
				onCheckedChanged: SettingsModel.developerMode = checked
			}

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Layout style") + SettingsModel.translationTrigger
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

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Create dummy entries") + SettingsModel.translationTrigger
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
}
