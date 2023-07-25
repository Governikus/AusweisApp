/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Governikus.Global 1.0
import Governikus.HistoryView 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.RemoteServiceView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.RemoteServiceModel 1.0
import Governikus.Type.HistoryModel 1.0
import Governikus.Type.LogModel 1.0
import Governikus.Type.WorkflowModel 1.0

SectionPage {
	id: baseItem
	function platformId(pName) {
		return "mobile," + pName.replace(" ", "").toLowerCase();
	}

	//: LABEL ANDROID IOS
	title: qsTr("Settings")

	content: Column {
		id: mainColumn
		spacing: 0
		width: baseItem.width

		TitledSeparator {
			contentMarginTop: Constants.component_spacing
			//: LABEL ANDROID IOS
			title: qsTr("General")
			width: parent.width
		}
		MenuItem {
			description: {
				switch (SettingsModel.language) {
				case "de":
					return "Deutsch";
				case "ru":
					return "Русский";
				case "uk":
					return "Українська";
				default:
					return "English";
				}
			}
			icon: "qrc:///images/location_flag_%1.svg".arg(SettingsModel.language)
			tintIcon: false
			//: LABEL ANDROID IOS
			title: qsTr("Change language")
			width: parent.width

			onClicked: popup.open()

			LanguageSelectionPopup {
				id: popup
			}
		}
		MenuItem {
			description: (plugin.isTabletLayout ?
				//: LABEL ANDROID
				qsTr("Landscape") :
				//: LABEL ANDROID
				qsTr("Portrait"))
			icon: plugin.isTabletLayout ? "qrc:///images/android/stay_primary_landscape-24px.svg" : "qrc:///images/android/stay_primary_portrait-24px.svg"
			tintIcon: true
			//: LABEL ANDROID
			title: qsTr("Screen orientation")
			visible: Constants.is_layout_android
			width: parent.width

			onClicked: orientationPopup.open()

			ScreenOrientationSelectionPopup {
				id: orientationPopup
			}
		}
		TitledSeparator {
			//: LABEL ANDROID IOS
			title: qsTr("Smartphone as card reader")
			width: parent.width
		}
		ColumnLayout {
			id: serverNameBase
			Accessible.focusable: true
			Accessible.name: serverNameText.text
			Accessible.role: Accessible.Grouping
			width: parent.width

			GText {
				id: serverNameText
				Accessible.ignored: true
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
					focus = false;
					SettingsModel.serverName = text;
				}

				Layout.bottomMargin: Constants.component_spacing / 2
				Layout.fillWidth: true
				Layout.leftMargin: Constants.component_spacing
				Layout.rightMargin: Constants.component_spacing
				maximumLength: Constants.maximumDeviceNameLength
				text: SettingsModel.serverName

				onAccepted: saveInput()
				onFocusChanged: if (!focus)
					saveInput()
			}
		}
		LabeledSwitch {
			checked: SettingsModel.pinPadMode
			//: LABEL ANDROID IOS
			description: qsTr("Enter PIN on this device")

			//: LABEL ANDROID IOS
			title: qsTr("PIN pad mode")
			width: parent.width

			onCheckedChanged: SettingsModel.pinPadMode = checked
		}
		LabeledSwitch {
			checked: SettingsModel.showAccessRights
			//: LABEL ANDROID IOS
			description: qsTr("Show requested rights on this device as well")
			enabled: SettingsModel.pinPadMode

			//: LABEL ANDROID IOS
			title: qsTr("Show access rights")
			width: parent.width

			onCheckedChanged: SettingsModel.showAccessRights = checked
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Manage paired devices and add new devices")

			//: LABEL ANDROID IOS
			title: qsTr("Manage pairings")
			width: parent.width

			onClicked: push(remoteServiceSettings)

			Component {
				id: remoteServiceSettings
				RemoteServiceSettings {
					Component.onCompleted: RemoteServiceModel.detectRemoteDevices = true
					Component.onDestruction: RemoteServiceModel.detectRemoteDevices = false
				}
			}
		}
		TitledSeparator {

			//: LABEL ANDROID IOS
			title: qsTr("Security and privacy")
			width: parent.width
		}
		LabeledSwitch {
			checked: SettingsModel.historyEnabled
			//: LABEL ANDROID IOS
			description: qsTr("Save authentication history")

			//: LABEL ANDROID IOS
			title: qsTr("Save history")
			width: parent.width

			onCheckedChanged: SettingsModel.historyEnabled = checked
		}
		MenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("View authentication history")

			//: LABEL ANDROID IOS
			title: qsTr("History")
			visible: WorkflowModel.isSmartSupported
			width: parent.width

			onClicked: push(historyView)

			Component {
				id: historyView
				HistoryView {
				}
			}
		}
		LabeledSwitch {
			checked: SettingsModel.shuffleScreenKeyboard
			//: LABEL ANDROID IOS
			description: qsTr("Randomize the order of the on screen keypad buttons")

			//: LABEL ANDROID IOS
			title: qsTr("Shuffle keypad buttons")
			width: parent.width

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}
		LabeledSwitch {
			checked: !SettingsModel.visualPrivacy
			//: LABEL ANDROID IOS
			description: qsTr("Visual feedback when pressing keypad buttons")

			//: LABEL ANDROID IOS
			title: qsTr("Keypad animations")
			width: parent.width

			onCheckedChanged: SettingsModel.visualPrivacy = !checked
		}
		Column {
			spacing: parent.spacing
			visible: SettingsModel.advancedSettings
			width: parent.width

			TitledSeparator {

				//: LABEL ANDROID IOS
				title: qsTr("CAN allowed mode")
				width: parent.width
			}
			LabeledSwitch {
				checked: SettingsModel.enableCanAllowed
				//: LABEL ANDROID IOS
				description: qsTr("Allow the id card to be used with only the CAN")

				//: LABEL ANDROID IOS
				title: qsTr("Support CAN allowed mode")
				width: parent.width

				onCheckedChanged: SettingsModel.enableCanAllowed = checked
			}
			LabeledSwitch {
				checked: SettingsModel.skipRightsOnCanAllowed
				//: LABEL ANDROID IOS
				description: qsTr("Do not show the rights page, when in can allowed mode")
				enabled: SettingsModel.enableCanAllowed

				//: LABEL ANDROID IOS
				title: qsTr("Skip rights page")
				width: parent.width

				onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
			}
			TitledSeparator {

				//: LABEL ANDROID IOS
				title: qsTr("Developer options")
				width: parent.width
			}
			LabeledSwitch {
				id: testUriSwitch
				checked: SettingsModel.useSelfauthenticationTestUri
				//: LABEL ANDROID IOS
				description: qsTr("Use the test environment during a self-authentication")

				//: LABEL ANDROID IOS
				title: qsTr("Testmode for the self-authentication")
				width: parent.width

				onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
			}
			LabeledSwitch {
				checked: SettingsModel.enableSimulator
				//: LABEL ANDROID IOS
				description: qsTr("Enable internal card simulator")

				//: LABEL ANDROID IOS
				title: qsTr("Internal card simulator")
				width: parent.width

				onCheckedChanged: SettingsModel.enableSimulator = checked
			}
		}
		Column {
			spacing: parent.spacing
			visible: plugin.debugBuild
			width: parent.width

			TitledSeparator {

				//: LABEL ANDROID IOS
				title: qsTr("Debug options")
				width: parent.width
			}
			LabeledSwitch {
				checked: SettingsModel.developerMode
				//: LABEL ANDROID IOS
				description: qsTr("Use a more tolerant mode")

				//: LABEL ANDROID IOS
				title: qsTr("Developer mode")
				width: parent.width

				onCheckedChanged: SettingsModel.developerMode = checked
			}
			TitledSeparator {

				//: LABEL ANDROID IOS
				title: qsTr("Layout style")
				width: parent.width
			}
			Column {
				bottomPadding: Constants.component_spacing / 2
				leftPadding: Constants.component_spacing
				rightPadding: Constants.component_spacing
				topPadding: Constants.component_spacing / 2
				width: parent.width

				GRadioButton {
					checked: plugin.platformStyle === baseItem.platformId(text)
					text: "Phone, iOS"

					onCheckedChanged: if (checked) {
						plugin.applyPlatformStyle(baseItem.platformId(text));
					}
				}
				GRadioButton {
					checked: plugin.platformStyle === baseItem.platformId(text)
					text: "Phone, Android"

					onCheckedChanged: if (checked) {
						plugin.applyPlatformStyle(baseItem.platformId(text));
					}
				}
				GRadioButton {
					checked: plugin.platformStyle === baseItem.platformId(text)
					text: "Tablet, iOS"

					onCheckedChanged: if (checked) {
						plugin.applyPlatformStyle(baseItem.platformId(text));
					}
				}
				GRadioButton {
					checked: plugin.platformStyle === baseItem.platformId(text)
					text: "Tablet, Android"

					onCheckedChanged: if (checked) {
						plugin.applyPlatformStyle(baseItem.platformId(text));
					}
				}
			}
			TitledSeparator {

				//: LABEL ANDROID IOS
				title: qsTr("Create dummy entries")
				width: parent.width
			}
			ColumnLayout {
				spacing: 0
				width: parent.width

				GButton {
					Layout.fillWidth: true
					Layout.margins: Constants.component_spacing
					Layout.topMargin: Constants.component_spacing / 2

					//: LABEL ALL_PLATFORMS
					text: qsTr("New Logfile")

					onClicked: {
						LogModel.saveDummyLogFile();
						ApplicationModel.showFeedback("Created new logfile.");
					}
				}
				GButton {
					Layout.fillWidth: true
					Layout.margins: Constants.component_spacing
					Layout.topMargin: 0

					//: LABEL ALL_PLATFORMS
					text: qsTr("15 days old Logfile")

					onClicked: {
						let date = new Date();
						date.setDate(new Date().getDate() - 15);
						LogModel.saveDummyLogFile(date);
						ApplicationModel.showFeedback("Created old logfile.");
					}
				}
				GButton {
					Layout.fillWidth: true
					Layout.margins: Constants.component_spacing
					Layout.topMargin: 0

					//: LABEL ALL_PLATFORMS
					text: qsTr("History")

					onClicked: {
						HistoryModel.createDummyEntry();
						ApplicationModel.showFeedback("Created new history entry.");
					}
				}
			}
		}
	}
}
