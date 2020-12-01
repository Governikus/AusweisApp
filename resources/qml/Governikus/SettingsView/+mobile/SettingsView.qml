/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.LogModel 1.0

SectionPage {
	id: baseItem

	navigationAction: NavigationAction { state: topLevelPage ? "" : "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Settings")

	function platformId(pName) {
		return "mobile," + pName.replace(" ", "").toLowerCase()
	}

	content: Column {
		id: mainColumn

		width: baseItem.width
		spacing: 0

		TitledSeparator {
			width: parent.width
			contentMarginTop: Constants.component_spacing
			//: LABEL ANDROID IOS
			title: qsTr("General")
		}

		MenuItem {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Change language")
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

		MenuItem {
			visible: Constants.is_layout_android
			width: parent.width
			//: LABEL ANDROID
			title: qsTr("Screen orientation")
			description: (Screen.primaryOrientation === Qt.LandscapeOrientation
				//: LABEL ANDROID
				? qsTr("Landscape")
				//: LABEL ANDROID
				: qsTr("Portrait")
			)
			icon: Screen.primaryOrientation === Qt.LandscapeOrientation ? "qrc:///images/android/stay_primary_landscape-24px.svg" : "qrc:///images/android/stay_primary_portrait-24px.svg"
			tintIcon: true

			onClicked: orientationPopup.open()

			ScreenOrientationSelectionPopup {
				id: orientationPopup
			}
		}

		TitledSeparator {
			width: parent.width
			//: LABEL ANDROID IOS
			title: qsTr("Smartphone as card reader")
		}

		ColumnLayout {
			id: serverNameBase

			width: parent.width

			GText {
				id: serverNameText

				Layout.fillWidth: true
				Layout.leftMargin: Constants.component_spacing
				Layout.rightMargin: Constants.component_spacing
				Layout.topMargin: Constants.component_spacing / 2

				//: LABEL ANDROID IOS
				text: qsTr("Device name")
				textStyle: Style.text.normal_accent
			}

			GTextField {
				id: serverName

				function saveInput() {
					focus = false
					SettingsModel.serverName = text
					text = SettingsModel.serverName
				}

				Layout.fillWidth: true
				Layout.leftMargin: Constants.component_spacing
				Layout.rightMargin: Constants.component_spacing
				Layout.bottomMargin: Constants.component_spacing / 2

				Component.onCompleted: text = SettingsModel.serverName

				maximumLength: Constants.maximumDeviceNameLength
				onAccepted: saveInput()
				onFocusChanged: if (!focus) saveInput()
				onVisibleChanged: if (!visible) saveInput()
			}
		}

		LabeledSwitch {
			width: parent.width

			//: LABEL ANDROID IOS
			title: qsTr("PIN pad mode")
			//: LABEL ANDROID IOS
			description: qsTr("Enter PIN on this device")
			checked: SettingsModel.pinPadMode
			onCheckedChanged: SettingsModel.pinPadMode = checked
		}

		MenuItem {
			width: parent.width

			//: LABEL ANDROID IOS
			title: qsTr("Remote card reader")
			//: LABEL ANDROID IOS
			description: qsTr("Configure remote service for another device")
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
			width: parent.width

			//: LABEL ANDROID IOS
			title: qsTr("Security and privacy")
		}

		LabeledSwitch {
			width: parent.width

			//: LABEL ANDROID IOS
			title: qsTr("History")
			//: LABEL ANDROID IOS
			description: qsTr("Save authentication history")
			checked: SettingsModel.historyEnabled
			onCheckedChanged: SettingsModel.historyEnabled = checked
		}

		LabeledSwitch {
			width: parent.width

			//: LABEL ANDROID IOS
			title: qsTr("Shuffle keypad buttons")
			//: LABEL ANDROID IOS
			description: qsTr("Randomize the order of the on screen keypad buttons")

			checked: SettingsModel.shuffleScreenKeyboard
			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}

		LabeledSwitch {
			width: parent.width

			//: LABEL ANDROID IOS
			title: qsTr("Keypad animations")
			//: LABEL ANDROID IOS
			description: qsTr("Visual feedback when pressing keypad buttons")
			checked: !SettingsModel.visualPrivacy
			onCheckedChanged: SettingsModel.visualPrivacy = !checked
		}

		Column {
			visible: SettingsModel.advancedSettings
			width: parent.width

			spacing: parent.spacing

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("CAN allowed mode")
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Support CAN allowed mode")
				//: LABEL ANDROID IOS
				description: qsTr("Allow the id card to be used with only the CAN")
				checked: SettingsModel.enableCanAllowed
				onCheckedChanged: SettingsModel.enableCanAllowed = checked
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Skip rights page")
				//: LABEL ANDROID IOS
				description: qsTr("Do not show the rights page, when in can allowed mode")
				enabled: SettingsModel.enableCanAllowed
				checked: SettingsModel.skipRightsOnCanAllowed
				onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
			}
		}

		Column {
			visible: plugin.debugBuild
			width: parent.width

			spacing: parent.spacing

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Developer Options")
			}

			LabeledSwitch {
				id: testUriSwitch

				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Testmode for the self-authentication")
				//: LABEL ANDROID IOS
				description: qsTr("Use the test environment during a self-authentication")

				checked: SettingsModel.useSelfauthenticationTestUri
				onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
			}

			LabeledSwitch {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Developer Mode")
				//: LABEL ANDROID IOS
				description: qsTr("Use a more tolerant mode")
				checked: SettingsModel.developerMode
				onCheckedChanged: SettingsModel.developerMode = checked
			}

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Layout style")
			}

			Column {
				width: parent.width
				leftPadding: Constants.component_spacing
				rightPadding: Constants.component_spacing
				topPadding: Constants.component_spacing / 2
				bottomPadding: Constants.component_spacing / 2

				GRadioButton {
					text: "Phone, iOS"
					checked: plugin.platformStyle === baseItem.platformId(text)
					onCheckedChanged: if (checked) { plugin.applyPlatformStyle(baseItem.platformId(text)) }
				}

				GRadioButton {
					text: "Phone, Android"
					checked: plugin.platformStyle === baseItem.platformId(text)
					onCheckedChanged: if (checked) { plugin.applyPlatformStyle(baseItem.platformId(text)) }
				}

				GRadioButton {
					text: "Tablet, iOS"
					checked: plugin.platformStyle === baseItem.platformId(text)
					onCheckedChanged: if (checked) { plugin.applyPlatformStyle(baseItem.platformId(text)) }
				}

				GRadioButton {
					text: "Tablet, Android"
					checked: plugin.platformStyle === baseItem.platformId(text)
					onCheckedChanged: if (checked) { plugin.applyPlatformStyle(baseItem.platformId(text)) }
				}
			}

			TitledSeparator {
				width: parent.width

				//: LABEL ANDROID IOS
				title: qsTr("Create dummy entries")
			}

			ColumnLayout {
				width: parent.width

				spacing: 0

				GButton {
					Layout.fillWidth: true
					Layout.margins: Constants.component_spacing
					Layout.topMargin: Constants.component_spacing / 2

					//: LABEL ALL_PLATFORMS
					text: qsTr("Logfile")
					onClicked: {
						LogModel.saveDummyLogfile()
						ApplicationModel.showFeedback("Created new logfile.")
					}
				}

				GButton {
					Layout.fillWidth: true
					Layout.margins: Constants.component_spacing
					Layout.topMargin: 0

					//: LABEL ALL_PLATFORMS
					text: qsTr("History")
					onClicked: {
						HistoryModel.createDummyEntry()
						ApplicationModel.showFeedback("Created new history entry.")
					}
				}
			}
		}
	}
}
