/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Window

import Governikus.Global
import Governikus.View
import Governikus.RemoteServiceView
import Governikus.Type
import Governikus.Style

FlickableSectionPage {
	id: root

	enableTileStyle: false
	spacing: Style.dimens.pane_spacing
	//: MOBILE
	title: qsTr("Settings")

	GOptionsContainer {
		//: MOBILE
		title: qsTr("General")

		GRadioGroup {
			id: languageCollapsible

			contentBottomMargin: 0
			contentHorizontalMargin: 0
			contentSpacing: 0
			contentTopMargin: 0
			drawTopCorners: true
			//: MOBILE
			title: qsTr("Change language")

			LanguageButtons {
				onButtonClicked: languageCollapsible.onOptionSelected()
			}
		}
		SettingsViewSeparator {
		}
		GRadioGroup {
			id: appearanceCollapsible

			contentBottomMargin: 0
			contentHorizontalMargin: 0
			contentSpacing: 0
			contentTopMargin: 0
			tintIcon: true
			//: MOBILE
			title: qsTr("Appearance")

			DarkModeButtons {
				width: parent.width

				onButtonClicked: appearanceCollapsible.onOptionSelected()
			}
		}
		SettingsViewSeparator {
		}
		GSwitch {
			//: MOBILE
			description: qsTr("Toggling will restart the %1").arg(Qt.application.name)
			//: MOBILE
			text: qsTr("Use system font")

			Component.onCompleted: {
				checked = SettingsModel.useSystemFont;
			}
			onCheckedChanged: {
				if (checked !== SettingsModel.useSystemFont) {
					SettingsModel.useSystemFont = checked;
					refreshPopup.open();
				}
			}

			ConfirmationPopup {
				id: refreshPopup

				//: MOBILE
				cancelButtonText: qsTr("Later")
				okButtonText: Qt.platform.os === "ios" ?
				//: IOS
				qsTr("Restart") :
				//: ANDROID
				qsTr("Restart now")
				//: MOBILE
				text: qsTr("The font change applies only after restarting the application.")
				//: MOBILE
				title: qsTr("Restart required")

				onConfirmed: UiPluginModel.doRefresh()
			}
		}
		SettingsViewSeparator {
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
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: MOBILE
		title: qsTr("Accessibility")

		GSwitch {
			checked: !SettingsModel.useAnimations
			drawTopCorners: true

			//: MOBILE
			text: qsTr("Use images instead of animations")

			onCheckedChanged: SettingsModel.useAnimations = !checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.visualPrivacy
			//: MOBILE
			text: qsTr("Hide key animations when entering PIN")

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: !SettingsModel.autoRedirectAfterAuthentication
			//: MOBILE
			description: qsTr("After identification, you will only be redirected back to the provider after confirmation. Otherwise, you will be redirected automatically after a few seconds.")
			drawBottomCorners: true
			//: MOBILE
			text: qsTr("Manual redirection back to the provider")

			onCheckedChanged: SettingsModel.autoRedirectAfterAuthentication = !checked
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: MOBILE
		title: qsTr("Smartphone as card reader")

		GCollapsible {
			alwaysReserveSelectionTitleHeight: true
			contentBottomMargin: 0
			contentTopMargin: 0
			drawTopCorners: true
			selectionTitle: expanded ? "" : SettingsModel.deviceName
			//: MOBILE
			title: qsTr("Device name")

			GTextField {
				function saveInput() {
					focus = false;
					SettingsModel.deviceName = text;
				}

				Layout.margins: Style.dimens.pane_spacing
				maximumLength: 33
				text: SettingsModel.deviceName

				onAccepted: saveInput()
				onFocusChanged: if (!focus)
					saveInput()
			}
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.pinPadMode

			//: MOBILE
			text: qsTr("Enter PIN on this device")

			onCheckedChanged: SettingsModel.pinPadMode = checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.showAccessRights
			enabled: SettingsModel.pinPadMode

			//: MOBILE
			text: qsTr("Show requested rights on this device as well")

			onCheckedChanged: SettingsModel.showAccessRights = checked
		}
		SettingsViewSeparator {
		}
		GMenuItem {
			Layout.fillWidth: true
			//: MOBILE
			description: qsTr("Add and remove devices")
			drawBottomCorners: true

			//: MOBILE
			title: qsTr("Manage pairings")

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
		//: ANDROID
		title: qsTr("Security and privacy")

		GSwitch {
			checked: SettingsModel.shuffleScreenKeyboard
			//: MOBILE
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")
			drawTopCorners: true

			//: MOBILE
			text: qsTr("Shuffle keys")

			onCheckedChanged: SettingsModel.shuffleScreenKeyboard = checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.visualPrivacy
			//: MOBILE
			description: qsTr("Makes it difficult for outsiders to detect PIN entry")
			drawBottomCorners: true

			//: MOBILE
			text: qsTr("Hide key animations")

			onCheckedChanged: SettingsModel.visualPrivacy = checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.screenPrivacy
			description: {
				if (Qt.platform.os === "android") {
					//: ANDROID
					return qsTr("Screenshots are prevented while your ID card data is displayed and you enter your PIN/CAN/PUK. If your screen is recorded, you will be notified of the potential collection of sensitive data.");
				}
				//: IOS
				return qsTr("In the event of a screenshot or video recording of the screen, you will be notified about the possible collection of sensitive data.");
			}
			drawBottomCorners: true
			drawTopCorners: true
			text: {
				if (Qt.platform.os === "android") {
					//: ANDROID
					return qsTr("Prevent screenshots");
				}
				//: IOS
				return qsTr("Detect screen recording");
			}

			onCheckedChanged: SettingsModel.screenPrivacy = checked
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: MOBILE
		title: qsTr("On-site reading")
		visible: SettingsModel.advancedSettings

		GSwitch {
			checked: SettingsModel.enableCanAllowed
			drawTopCorners: true

			//: MOBILE
			text: qsTr("Support CAN allowed mode for on-site reading")

			onCheckedChanged: SettingsModel.enableCanAllowed = checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.skipRightsOnCanAllowed
			drawBottomCorners: true
			enabled: SettingsModel.enableCanAllowed

			//: MOBILE
			text: qsTr("Skip rights page")

			onCheckedChanged: SettingsModel.skipRightsOnCanAllowed = checked
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true
		//: MOBILE
		title: qsTr("Developer options")
		visible: SettingsModel.advancedSettings

		GSwitch {
			Layout.fillWidth: true
			checked: SettingsModel.useSelfauthenticationTestUri
			//: MOBILE
			description: qsTr("Allow test sample card usage")
			drawTopCorners: true

			//: MOBILE
			text: qsTr("Testmode for the self-authentication")

			onCheckedChanged: SettingsModel.useSelfauthenticationTestUri = checked
		}
		SettingsViewSeparator {
		}
		GSwitch {
			checked: SettingsModel.enableSimulator
			//: MOBILE
			description: qsTr("Simulate a test sample card in authentications")
			drawBottomCorners: true

			//: MOBILE
			text: qsTr("Internal card simulator")

			onCheckedChanged: SettingsModel.enableSimulator = checked
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: MOBILE
		title: qsTr("Debug options")
		visible: UiPluginModel.debugBuild

		GSwitch {
			checked: SettingsModel.developerMode
			//: MOBILE
			description: qsTr("Use a more tolerant mode")
			drawTopCorners: true

			//: MOBILE
			text: qsTr("Developer mode")

			onCheckedChanged: SettingsModel.developerMode = checked
		}
		SettingsViewSeparator {
		}
		GMenuItem {
			Layout.fillWidth: true
			//: MOBILE
			description: qsTr("Show Transport PIN reminder, store feedback and close reminder dialogs.")
			drawBottomCorners: true
			icon.source: "qrc:///images/material_refresh.svg"
			//: MOBILE
			title: qsTr("Reset hideable dialogs")

			onClicked: SettingsModel.resetHideableDialogs()
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: MOBILE
		title: qsTr("Extend Transport PIN")
		visible: UiPluginModel.debugBuild

		Flow {
			Layout.fillWidth: true
			padding: Style.dimens.pane_padding
			spacing: Style.dimens.pane_spacing

			SecondaryButton {
				Layout.minimumWidth: implicitHeight
				checkable: true
				checked: SettingsModel.appendTransportPin === ""
				//: MOBILE
				text: qsTr("Disable")

				onClicked: SettingsModel.appendTransportPin = ""
			}
			Repeater {
				model: ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]

				SecondaryButton {
					required property string modelData

					Layout.minimumWidth: implicitHeight
					checkable: true
					checked: SettingsModel.appendTransportPin === modelData
					text: modelData

					onClicked: SettingsModel.appendTransportPin = modelData
				}
			}
		}
	}
	GOptionsContainer {
		Layout.fillWidth: true

		//: MOBILE
		title: qsTr("Create dummy entries")
		visible: UiPluginModel.debugBuild

		ColumnLayout {
			spacing: Style.dimens.pane_spacing

			GButton {
				Layout.leftMargin: Style.dimens.pane_padding
				Layout.rightMargin: Style.dimens.pane_padding
				Layout.topMargin: Style.dimens.pane_padding

				//: ALL_PLATFORMS
				text: qsTr("New Logfile")

				onClicked: {
					LogFilesModel.saveDummyLogFile();
					ApplicationModel.showFeedback("Created new logfile.");
				}
			}
			GButton {
				Layout.bottomMargin: Style.dimens.pane_padding
				Layout.leftMargin: Style.dimens.pane_padding
				Layout.rightMargin: Style.dimens.pane_padding

				//: ALL_PLATFORMS
				text: qsTr("15 days old Logfile")

				onClicked: {
					let date = new Date();
					date.setDate(new Date().getDate() - 15);
					LogFilesModel.saveDummyLogFile(date);
					ApplicationModel.showFeedback("Created old logfile.");
				}
			}
		}
	}

	component SettingsViewSeparator: GSeparator {
		Layout.fillWidth: true
		Layout.leftMargin: Style.dimens.pane_spacing
		Layout.rightMargin: Layout.leftMargin
	}
}
