/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import Governikus.Global
import Governikus.View
import Governikus.RemoteServiceView
import Governikus.SmartView
import Governikus.Type
import Governikus.Style

FlickableSectionPage {
	id: root

	function platformId(pName) {
		return "mobile," + pName.replace(" ", "").toLowerCase();
	}

	enableTileStyle: false
	spacing: Style.dimens.pane_spacing
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

			GRepeater {
				delegate: GCollapsibleSubButton {
					required property string a11yDescription
					required property string a11yName
					required property string language
					required property string languageText

					Accessible.description: a11yDescription
					Accessible.name: a11yName
					Layout.fillWidth: true
					text: languageText

					onClicked: {
						SettingsModel.language = language;
						languageCollapsible.expanded = false;
					}
				}
				model: LanguageButtonData {
				}
			}
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
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

			DarkModeButtons {
				id: modeButtons

				width: parent.width

				onButtonClicked: appearanceCollapsible.expanded = false
			}
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
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
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
			visible: technologySwitch.visible
		}
		TechnologySwitch {
			id: technologySwitch

			contentBottomMargin: 0
			contentHorizontalMargin: 0
			contentSpacing: 0
			contentTopMargin: 0
			drawBottomCorners: true
			visible: hasSelection
			width: parent.width
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Accessibility")

		GSwitch {
			checked: !SettingsModel.useAnimations
			drawTopCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Use images instead of animations")
			width: parent.width

			onCheckedChanged: SettingsModel.useAnimations = !checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GSwitch {
			checked: SettingsModel.visualPrivacy
			//: LABEL ANDROID IOS
			text: qsTr("Hide key animations when entering PIN")
			width: parent.width

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GSwitch {
			checked: !SettingsModel.autoRedirectAfterAuthentication
			//: LABEL ANDROID IOS
			description: qsTr("After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.")
			drawBottomCorners: true
			//: LABEL ANDROID IOS
			text: qsTr("Manual redirection back to the provider")
			width: parent.width

			onCheckedChanged: SettingsModel.autoRedirectAfterAuthentication = !checked
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

			GTextField {
				function saveInput() {
					focus = false;
					SettingsModel.deviceName = text;
				}

				Layout.fillWidth: true
				Layout.margins: Style.dimens.pane_spacing
				maximumLength: 33
				text: SettingsModel.deviceName

				onAccepted: saveInput()
				onFocusChanged: focus ? root.positionViewAtItem(this) : saveInput()
			}
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GSwitch {
			checked: SettingsModel.pinPadMode

			//: LABEL ANDROID IOS
			text: qsTr("Enter PIN on this device")
			width: parent.width

			onCheckedChanged: SettingsModel.pinPadMode = checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GSwitch {
			checked: SettingsModel.showAccessRights
			enabled: SettingsModel.pinPadMode

			//: LABEL ANDROID IOS
			text: qsTr("Show requested rights on this device as well")
			width: parent.width

			onCheckedChanged: SettingsModel.showAccessRights = checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Add and remove devices")
			drawBottomCorners: true

			//: LABEL ANDROID IOS
			title: qsTr("Manage pairings")
			width: parent.width

			onClicked: root.push(remoteServiceSettings)

			Component {
				id: remoteServiceSettings

				RemoteServiceSettings {
					enableTileStyle: root.enableTileStyle

					Component.onCompleted: RemoteServiceModel.startDetection()
					Component.onDestruction: RemoteServiceModel.stopDetection(true)
				}
			}
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Numeric keypad")

		GSwitch {
			checked: SettingsModel.shuffleScreenKeyboard
			//: LABEL ANDROID IOS
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")
			drawTopCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Shuffle keys")
			width: parent.width

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GSwitch {
			checked: SettingsModel.visualPrivacy
			//: LABEL ANDROID IOS
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")
			drawBottomCorners: true

			//: LABEL ANDROID IOS
			text: qsTr("Hide key animations")
			width: parent.width

			onCheckedChanged: SettingsModel.visualPrivacy = checked
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

			onClicked: root.push(smartDeleteView)

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
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
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
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
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
		}
		GSeparator {
			anchors.left: parent.left
			anchors.leftMargin: Style.dimens.pane_spacing
			anchors.right: parent.right
			anchors.rightMargin: Style.dimens.pane_spacing
		}
		GMenuItem {
			//: LABEL ANDROID IOS
			description: qsTr("Show Transport PIN reminder, store feedback and close reminder dialogs.")
			drawBottomCorners: true
			icon.source: "qrc:///images/material_refresh.svg"
			//: LABEL ANDROID IOS
			title: qsTr("Reset hideable dialogs")
			width: parent.width

			onClicked: SettingsModel.resetHideableDialogs()
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: LABEL ANDROID IOS
		title: qsTr("Extend Transport PIN")
		visible: UiPluginModel.debugBuild

		Flow {
			padding: Style.dimens.pane_padding
			spacing: Style.dimens.pane_spacing
			width: parent.width

			GButton {
				Layout.minimumWidth: implicitHeight
				checkable: true
				checked: SettingsModel.appendTransportPin === ""
				style: Style.color.controlOptional
				//: LABEL ANDROID IOS
				text: qsTr("Disable")

				onClicked: SettingsModel.appendTransportPin = ""
			}
			Repeater {
				model: ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]

				GButton {
					required property string modelData

					Layout.minimumWidth: implicitHeight
					checkable: true
					checked: SettingsModel.appendTransportPin === modelData
					style: Style.color.controlOptional
					text: modelData

					onClicked: SettingsModel.appendTransportPin = modelData
				}
			}
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: LABEL ANDROID IOS
		title: qsTr("Create dummy entries")
		visible: UiPluginModel.debugBuild

		ColumnLayout {
			spacing: Style.dimens.pane_spacing
			width: parent.width

			GButton {
				Layout.leftMargin: Style.dimens.pane_padding
				Layout.rightMargin: Style.dimens.pane_padding
				Layout.topMargin: Style.dimens.pane_padding

				//: LABEL ALL_PLATFORMS
				text: qsTr("New Logfile")

				onClicked: {
					LogModel.saveDummyLogFile();
					ApplicationModel.showFeedback("Created new logfile.");
				}
			}
			GButton {
				Layout.bottomMargin: Style.dimens.pane_padding
				Layout.leftMargin: Style.dimens.pane_padding
				Layout.rightMargin: Style.dimens.pane_padding

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
