/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Controls 2.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.ResultView 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.SettingsView 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: baseItem

	enum SubViews {
		Welcome = 0,
		AutoStartSetting,
		HistorySetting,
		CardReaderInfo,
		CardReader,
		TransportPin,
		PasswordInfo,
		Finished
	}

	isAbstract: true

	onVisibleChanged: if (visible) d.reset()
	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Setup Assistant")
		rootEnabled: d.allowNavigation
		showSettings: false
		helpTopic: "setupAssistant"
		onClicked: d.reset()
	}

	QtObject {
		id: d

		property int activeView: SetupAssistantView.SubViews.Welcome
		readonly property int startupModule: SettingsModel.startupModule
		readonly property bool allowNavigation: startupModule !== UiModule.TUTORIAL

		function reset() {
			d.activeView = SetupAssistantView.SubViews.Welcome
		}
	}

	DecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.Welcome

		Component.onCompleted: setActive()

		mainIconSource: "qrc:///images/status_info.svg"
		//: INFO DESKTOP Welcome message when starting the setup assistant.
		questionText: qsTr("Welcome to the AusweisApp2. Please take a few moments to set up the environment to your needs. Every decision you make can later be changed in the settings menu.")
		style: DecisionView.ButtonStyle.AgreeButton
		agreeText: ""
		agreeButton.iconSource: "qrc:///images/desktop/material_arrow_forward.svg"

		onAgree: d.activeView = SettingsModel.autoStartAvailable ? SetupAssistantView.SubViews.AutoStartSetting : SetupAssistantView.SubViews.HistorySetting
	}

	DecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.AutoStartSetting

		mainIconSource: "qrc:///images/status_info.svg"
		//: INFO DESKTOP Question if the App shall be started automatically after boot
		questionText: qsTr("Do you want to automatically start the %1 after boot?").arg(Qt.application.name)
		//: INFO DESKTOP Information text why autostart of the App is advisable
		questionSubText: {
			let subText = qsTr("In order to successfully use the online identification function, %1 has to be running. It is therefore advisable to activate the auto-start after system startup.").arg(Qt.application.name)
			if (Qt.platform.os === "osx") {
				//: INFO MACOS Additional information that macOS auto-start add a symbol to the menu bar
				subText += " " + qsTr("The launch will add an icon to the menu bar.")
			}
			return subText
		}

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP
			text: qsTr("Auto-start Setting")
			rootEnabled: d.allowNavigation
			showSettings: false
			helpTopic: "setupAssistant"
		}

		onAgree: {
			SettingsModel.autoStartApp = true
			d.activeView = SetupAssistantView.SubViews.HistorySetting
		}

		onDisagree: {
			SettingsModel.autoStartApp = false
			d.activeView = SetupAssistantView.SubViews.HistorySetting
		}
	}

	DecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.HistorySetting

		mainIconSource: "qrc:///images/material_history.svg"
		//: INFO DESKTOP Question if the authentication history shall be stored.
		questionText: qsTr("Do you want to save a history of performed authentications on your device?")
		//: INFO DESKTOP Information text which data is stored in the history record.
		questionSubText: qsTr("The history is only saved locally. You can use it to see on what date you transmitted which data to which party. After enabling the history you can view and delete the entries anytime.")

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP
			text: qsTr("History Setting")
			rootEnabled: d.allowNavigation
			showSettings: false
			helpTopic: "setupAssistant"
		}

		onAgree: {
			SettingsModel.historyEnabled = true
			d.activeView = SetupAssistantView.SubViews.CardReaderInfo
		}

		onDisagree: {
			SettingsModel.historyEnabled = false
			d.activeView = SetupAssistantView.SubViews.CardReaderInfo
		}
	}

	DecisionView {
		visible: d.activeView === SetupAssistantView.SubViews.CardReaderInfo

		tintEnabled: false
		mainIconSource: "qrc:///images/reader/default_reader.png"
		//: INFO DESKTOP Question if the the user wants to setup any card readers now.
		questionText: qsTr("Do you want to set up a card reader <u>now</u>?")
		//: INFO DESKTOP Information text why a card reader is required to use the online
		questionSubText: qsTr("In order to use the online identification feature on the computer, you need to set up a suitable smartphone or card reader before the first authentication process.")

		titleBarAction: TitleBarAction {
			//: LABEL DESKTOP
			text: qsTr("Card Readers")
			rootEnabled: d.allowNavigation
			showSettings: false
			helpTopic: "setupAssistant"
		}

		onAgree: d.activeView = SetupAssistantView.SubViews.CardReader

		onDisagree: d.activeView = SetupAssistantView.SubViews.TransportPin
	}

	TabbedReaderView {
		id: readerView

		visible: d.activeView === SetupAssistantView.SubViews.CardReader

		rootEnabled: d.allowNavigation

		onCloseView: d.activeView = SetupAssistantView.SubViews.CardReaderInfo

		paneAnchors.bottom: forwardButton.top

		NavigationButton {
			id: forwardButton

			visible: readerView.currentView === TabbedReaderView.SubView.None

			anchors {
				right: parent.right
				bottom: parent.bottom
				margins: Constants.pane_padding
			}

			buttonType: NavigationButton.Type.Forward
			onClicked: d.activeView = SetupAssistantView.SubViews.TransportPin
		}
	}

	TransportPinAssistantView {
		visible: d.activeView === SetupAssistantView.SubViews.TransportPin

		rootEnabled: d.allowNavigation

		onAgree: {
			SettingsModel.startupModule = UiModule.DEFAULT // We don't want to show the setup assistant again, as the only subview left is "Setup assistant done"
			SettingsModel.transportPinReminder = false
			baseItem.nextView(UiModule.PINMANAGEMENT)
		}
		onDisagree: {
			SettingsModel.startupModule = UiModule.DEFAULT
			d.activeView = SetupAssistantView.SubViews.Finished
		}
	}

	ResultView {
		visible: d.activeView === SetupAssistantView.SubViews.Finished

		resultType: ResultView.Type.IsSuccess
		//: INFO DESKTOP Success message after completing the setup assistant.
		text: qsTr("You have completed the setup of the AusweisApp2 successfully.")
		//: INFO DESKTOP A11y button text to exit the setup assistant.
		buttonText: qsTr("Proceed to start page")
		onNextView: pName => { baseItem.nextView(pName) }
	}

}
