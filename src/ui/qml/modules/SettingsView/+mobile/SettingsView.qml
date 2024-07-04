/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
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
import Governikus.Type

FlickableSectionPage {
	id: baseItem

	function platformId(pName) {
		return "mobile," + pName.replace(" ", "").toLowerCase();
	}

	enableTileStyle: false
	spacing: Constants.component_spacing
	//: LABEL ANDROID IOS
	title: qsTr("Settings")

	GOptionsContainer {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("General")

		GCollapsible {
			id: languageCollapsible

			contentBottomMargin: 0
			contentHorizontalMargin: 0
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

			onReceivedFocus: pItem => baseItem.positionViewAtItem(pItem)

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
					onFocusChanged: if (focus)
						baseItem.positionViewAtItem(this)
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
			contentHorizontalMargin: 0
			contentSpacing: 0
			contentTopMargin: 0
			selectionIcon: modeButtons.selectedIconPath
			selectionTitle: modeButtons.selectedText
			tintIcon: true
			//: LABEL ANDROID IOS
			title: qsTr("Appearance")
			width: parent.width

			onReceivedFocus: pItem => baseItem.positionViewAtItem(pItem)

			DarkModeButtons {
				id: modeButtons

				width: parent.width

				onButtonClicked: appearanceCollapsible.expanded = false
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			//: LABEL ANDROID IOS
			description: qsTr("Toggling will restart the %1").arg(Qt.application.name)

			//: LABEL ANDROID IOS
			text: qsTr("Use system font")
			width: parent.width

			Component.onCompleted: {
				checked = SettingsModel.useSystemFont;
			}
			onCheckedChanged: {
				if (checked !== SettingsModel.useSystemFont) {
					SettingsModel.useSystemFont = checked;
					UiPluginModel.doRefresh();
				}
			}
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			checked: SettingsModel.useAnimations
			drawBottomCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Play animations")
			width: parent.width

			onCheckedChanged: SettingsModel.useAnimations = checked
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Smartphone as card reader")

		GCollapsible {
			alwaysReserveSelectionTitleHeight: true
			contentBottomMargin: 0
			contentTopMargin: 0
			drawTopCorners: true
			selectionTitle: expanded ? "" : SettingsModel.deviceName
			//: LABEL ANDROID IOS
			title: qsTr("Device name")
			width: parent.width

			onReceivedFocus: pItem => baseItem.positionViewAtItem(pItem)

			GTextField {
				function saveInput() {
					focus = false;
					SettingsModel.deviceName = text;
				}

				Layout.fillWidth: true
				Layout.margins: Constants.component_spacing
				maximumLength: Constants.maximumDeviceNameLength
				text: SettingsModel.deviceName

				onAccepted: saveInput()
				onFocusChanged: focus ? baseItem.positionViewAtItem(this) : saveInput()
			}
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			checked: SettingsModel.pinPadMode

			//: LABEL ANDROID IOS
			text: qsTr("Enter PIN on this device")
			width: parent.width

			onCheckedChanged: SettingsModel.pinPadMode = checked
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			checked: SettingsModel.showAccessRights
			enabled: SettingsModel.pinPadMode

			//: LABEL ANDROID IOS
			text: qsTr("Show requested rights on this device as well")
			width: parent.width

			onCheckedChanged: SettingsModel.showAccessRights = checked
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Add and remove devices")
			drawBottomCorners: true

			//: LABEL ANDROID IOS
			title: qsTr("Manage pairings")
			width: parent.width

			onClicked: push(remoteServiceSettings)
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)

			Component {
				id: remoteServiceSettings

				RemoteServiceSettings {
					enableTileStyle: baseItem.enableTileStyle

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

		GSwitch {
			checked: SettingsModel.shuffleScreenKeyboard
			drawTopCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Randomize the order of the on screen keypad buttons")
			width: parent.width

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			checked: !SettingsModel.visualPrivacy
			//: LABEL ANDROID IOS
			description: qsTr("Visually highlight key presses on screen keypad")
			drawBottomCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Keypad animations")
			width: parent.width

			onCheckedChanged: SettingsModel.visualPrivacy = !checked
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Smart-eID")
		visible: ApplicationModel.isSmartSupported

		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Reset Smart-eID data on your device")
			//: LABEL ANDROID IOS
			title: qsTr("Reset Smart-eID")
			width: parent.width

			onClicked: push(smartDeleteView)
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)

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

		GSwitch {
			checked: SettingsModel.enableCanAllowed
			drawTopCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Support CAN allowed mode for on-site reading")
			width: parent.width

			onCheckedChanged: SettingsModel.enableCanAllowed = checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			checked: SettingsModel.skipRightsOnCanAllowed
			drawBottomCorners: true
			enabled: SettingsModel.enableCanAllowed

			//: LABEL ANDROID IOS
			text: qsTr("Skip rights page")
			width: parent.width

			onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Developer options")
		visible: SettingsModel.advancedSettings

		GSwitch {
			id: testUriSwitch

			checked: SettingsModel.useSelfauthenticationTestUri
			//: LABEL ANDROID IOS
			description: qsTr("Allow test sample card usage")
			drawTopCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Testmode for the self-authentication")
			width: parent.width

			onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GSwitch {
			checked: SettingsModel.enableSimulator
			//: LABEL ANDROID IOS
			description: qsTr("Simulate a test sample card in authentications")
			drawBottomCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Internal card simulator")
			width: parent.width

			onCheckedChanged: SettingsModel.enableSimulator = checked
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: LABEL ANDROID IOS
		title: qsTr("Debug options")
		visible: UiPluginModel.debugBuild

		GSwitch {
			checked: SettingsModel.developerMode
			//: LABEL ANDROID IOS
			description: qsTr("Use a more tolerant mode")
			drawTopCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Developer mode")
			width: parent.width

			onCheckedChanged: SettingsModel.developerMode = checked
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Constants.component_spacing
			anchors.right: parent.right
			anchors.rightMargin: Constants.component_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Show Transport PIN reminder, store feedback and close reminder dialogs.")
			drawBottomCorners: true
			icon.source: "qrc:///images/material_refresh.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Reset hideable dialogs")
			width: parent.width

			onClicked: {
				SettingsModel.resetHideableDialogs();
			}
			onFocusChanged: if (focus)
				baseItem.positionViewAtItem(this)
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: LABEL ANDROID IOS
		title: qsTr("Create dummy entries")
		visible: UiPluginModel.debugBuild

		ColumnLayout {
			spacing: Constants.component_spacing
			width: parent.width

			GButton {
				Layout.leftMargin: Constants.pane_padding
				Layout.rightMargin: Constants.pane_padding
				Layout.topMargin: Constants.pane_padding

				//: LABEL ALL_PLATFORMS
				text: qsTr("New Logfile")

				onClicked: {
					LogModel.saveDummyLogFile();
					ApplicationModel.showFeedback("Created new logfile.");
				}
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
			GButton {
				Layout.bottomMargin: Constants.pane_padding
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
				onFocusChanged: if (focus)
					baseItem.positionViewAtItem(this)
			}
		}
	}
}
