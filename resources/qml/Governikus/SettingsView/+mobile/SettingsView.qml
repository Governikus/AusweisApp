/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.RemoteServiceView
import Governikus.SmartView
import Governikus.Type.SettingsModel
import Governikus.Type.ApplicationModel
import Governikus.Type.RemoteServiceModel
import Governikus.Type.LogModel
import Governikus.Type.ModeOption
import Governikus.Type.WorkflowModel
import Governikus.Type.SmartModel

FlickableSectionPage {
	id: baseItem

	function platformId(pName) {
		return "mobile," + pName.replace(" ", "").toLowerCase();
	}

	//: LABEL ANDROID IOS
	title: qsTr("Settings")

	ColumnLayout {
		Layout.fillWidth: true
		spacing: Constants.component_spacing

		GOptionsContainer {
			Layout.fillWidth: true
			//: LABEL ANDROID IOS
			title: qsTr("General")

			GCollapsible {
				id: languageCollapsible

				contentBottomMargin: 0
				contentSpacing: 0
				contentTopMargin: 0
				drawTopCorners: true
				selectionIcon: "qrc:///images/location_flag_%1.svg".arg(SettingsModel.language)
				selectionTitle: {
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
				//: LABEL ANDROID IOS
				title: qsTr("Change language")
				width: parent.width

				GRepeater {
					id: repeater

					delegate: GCollapsibleSubButton {
						Accessible.description: model.a11yDescription
						Accessible.name: model.a11yName
						Layout.fillWidth: true
						image: model.image
						title: model.text

						onClicked: {
							SettingsModel.language = model.language;
							languageCollapsible.expanded = false;
						}
					}
					model: LanguageButtonData {
					}
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			GCollapsible {
				id: appearanceCollapsible

				contentBottomMargin: 0
				contentSpacing: 0
				contentTopMargin: 0
				selectionIcon: modeButtons.selectedIconPath
				selectionTitle: modeButtons.selectedText
				tintIcon: true
				//: LABEL ANDROID IOS
				title: qsTr("Appearance")
				width: parent.width

				DarkModeButtons {
					id: modeButtons

					width: parent.width

					onButtonClicked: appearanceCollapsible.expanded = false
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			LabeledSwitch {
				//: LABEL ANDROID IOS
				description: qsTr("Toggling will restart the %1").arg(Qt.application.name)
				drawBottomCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Use system font")
				width: parent.width

				Component.onCompleted: {
					checked = SettingsModel.useSystemFont;
				}
				onCheckedChanged: {
					if (checked !== SettingsModel.useSystemFont) {
						SettingsModel.useSystemFont = checked;
						plugin.doRefresh();
					}
				}
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true
			//: LABEL ANDROID IOS
			title: qsTr("Smartphone as card reader")

			GCollapsible {
				alwaysReserveSelectionTitleHight: true
				contentBottomMargin: 0
				contentTopMargin: 0
				drawTopCorners: true
				selectionTitle: expanded ? "" : SettingsModel.serverName
				//: LABEL ANDROID IOS
				title: qsTr("Device name")
				width: parent.width

				GTextField {
					id: serverName

					function saveInput() {
						focus = false;
						SettingsModel.serverName = text;
					}

					Layout.fillWidth: true
					Layout.margins: Constants.component_spacing
					maximumLength: Constants.maximumDeviceNameLength
					text: SettingsModel.serverName

					onAccepted: saveInput()
					onFocusChanged: if (!focus)
						saveInput()
				}
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			LabeledSwitch {
				checked: SettingsModel.pinPadMode

				//: LABEL ANDROID IOS
				title: qsTr("Enter PIN on this device")
				width: parent.width

				onCheckedChanged: SettingsModel.pinPadMode = checked
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			LabeledSwitch {
				checked: SettingsModel.showAccessRights
				enabled: SettingsModel.pinPadMode

				//: LABEL ANDROID IOS
				title: qsTr("Show requested rights on this device as well")
				width: parent.width

				onCheckedChanged: SettingsModel.showAccessRights = checked
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				//: LABEL ANDROID IOS
				description: qsTr("Add and remove devices")
				drawBottomCorners: true

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
		}
		GOptionsContainer {
			Layout.fillWidth: true
			//: LABEL ANDROID IOS
			title: qsTr("Security and privacy")

			LabeledSwitch {
				checked: SettingsModel.shuffleScreenKeyboard
				drawTopCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Randomize the order of the on screen keypad buttons")
				width: parent.width

				onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			LabeledSwitch {
				checked: !SettingsModel.visualPrivacy
				//: LABEL ANDROID IOS
				description: qsTr("Visually highlight key presses on screen keypad")
				drawBottomCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Keypad animations")
				width: parent.width

				onCheckedChanged: SettingsModel.visualPrivacy = !checked
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true
			//: LABEL ANDROID IOS
			title: qsTr("Smart-eID")
			visible: WorkflowModel.isSmartSupported

			MenuItem {
				//: LABEL ANDROID IOS
				description: qsTr("Reset Smart-eID data on your device")
				//: LABEL ANDROID IOS
				title: qsTr("Reset Smart-eID")
				width: parent.width

				onClicked: push(smartDeleteView)

				Component {
					id: smartDeleteView

					SmartResetView {
					}
				}
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true
			//: LABEL ANDROID IOS
			title: qsTr("On-site reading")
			visible: SettingsModel.advancedSettings

			LabeledSwitch {
				checked: SettingsModel.enableCanAllowed
				drawTopCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Support CAN allowed mode for on-site reading")
				width: parent.width

				onCheckedChanged: SettingsModel.enableCanAllowed = checked
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			LabeledSwitch {
				checked: SettingsModel.skipRightsOnCanAllowed
				drawBottomCorners: true
				enabled: SettingsModel.enableCanAllowed

				//: LABEL ANDROID IOS
				title: qsTr("Skip rights page")
				width: parent.width

				onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true
			//: LABEL ANDROID IOS
			title: qsTr("Developer options")
			visible: SettingsModel.advancedSettings

			LabeledSwitch {
				id: testUriSwitch

				checked: SettingsModel.useSelfauthenticationTestUri
				//: LABEL ANDROID IOS
				description: qsTr("Allow test sample card usage")
				drawTopCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Testmode for the self-authentication")
				width: parent.width

				onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			LabeledSwitch {
				checked: SettingsModel.enableSimulator
				//: LABEL ANDROID IOS
				description: qsTr("Simulate a test sample card in authentications")
				drawBottomCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Internal card simulator")
				width: parent.width

				onCheckedChanged: SettingsModel.enableSimulator = checked
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true

			//: LABEL ANDROID IOS
			title: qsTr("Debug options")
			visible: plugin.debugBuild

			LabeledSwitch {
				checked: SettingsModel.developerMode
				//: LABEL ANDROID IOS
				description: qsTr("Use a more tolerant mode")
				drawTopCorners: true

				//: LABEL ANDROID IOS
				title: qsTr("Developer mode")
				width: parent.width

				onCheckedChanged: SettingsModel.developerMode = checked
			}
			GSeparator {
				anchors.left: parent.left
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
			}
			MenuItem {
				//: LABEL ANDROID IOS
				description: qsTr("Show Transport PIN reminder, store feedback and close reminder dialogs.")
				drawBottomCorners: true
				icon: "qrc:///images/material_refresh.svg"
				//: LABEL ANDROID IOS
				title: qsTr("Reset hideable dialogs")
				width: parent.width

				onClicked: {
					SettingsModel.resetHideableDialogs();
				}
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true

			//: LABEL ANDROID IOS
			title: qsTr("Layout style")
			visible: plugin.debugBuild

			Column {
				padding: Constants.pane_padding
				width: parent.width

				GRadioButton {
					checked: plugin.platformStyle === baseItem.platformId(text)
					text: "iOS"

					onCheckedChanged: if (checked) {
						plugin.applyPlatformStyle(baseItem.platformId(text));
					}
				}
				GRadioButton {
					checked: plugin.platformStyle === baseItem.platformId(text)
					text: "Android"

					onCheckedChanged: if (checked) {
						plugin.applyPlatformStyle(baseItem.platformId(text));
					}
				}
			}
		}
		GOptionsContainer {
			Layout.fillWidth: true

			//: LABEL ANDROID IOS
			title: qsTr("Create dummy entries")
			visible: plugin.debugBuild

			ColumnLayout {
				spacing: Constants.component_spacing
				width: parent.width

				GButton {
					Layout.fillWidth: true
					Layout.leftMargin: Constants.pane_padding
					Layout.rightMargin: Constants.pane_padding
					Layout.topMargin: Constants.pane_padding

					//: LABEL ALL_PLATFORMS
					text: qsTr("New Logfile")

					onClicked: {
						LogModel.saveDummyLogFile();
						ApplicationModel.showFeedback("Created new logfile.");
					}
				}
				GButton {
					Layout.bottomMargin: Constants.pane_padding
					Layout.fillWidth: true
					Layout.leftMargin: Constants.pane_padding
					Layout.rightMargin: Constants.pane_padding

					//: LABEL ALL_PLATFORMS
					text: qsTr("15 days old Logfile")

					onClicked: {
						let date = new Date();
						date.setDate(new Date().getDate() - 15);
						LogModel.saveDummyLogFile(date);
						ApplicationModel.showFeedback("Created old logfile.");
					}
				}
			}
		}
	}
}
